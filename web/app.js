"use strict";

/* ---------- data (same as the C++ version) ---------- */

const LOCATIONS = [
  { id: 1, name: "DHA" },
  { id: 2, name: "Sadar Bazaar" },
  { id: 3, name: "Anarkali" },
  { id: 4, name: "Model Town" },
  { id: 5, name: "Gulberg" },
  { id: 6, name: "Defence Road" },
  { id: 7, name: "Canal Road" },
  { id: 8, name: "Mall Road" },
  { id: 9, name: "Johar Town" },
  { id: 10, name: "Bahria Town" },
];

const DEFAULT_ROADS = [
  [1, 2, 8, "Normal"], [2, 3, 6, "Normal"], [3, 4, 12, "Heavy Traffic"], [4, 5, 5, "Normal"],
  [5, 6, 9, "Normal"], [6, 7, 7, "Heavy Traffic"], [7, 8, 4, "Normal"], [8, 9, 10, "Blocked"],
  [9, 10, 14, "Normal"], [10, 1, 18, "Heavy Traffic"], [1, 3, 14, "Normal"], [2, 4, 15, "Normal"],
  [3, 5, 11, "Normal"], [4, 6, 8, "Heavy Traffic"], [5, 7, 13, "Normal"], [6, 8, 9, "Normal"],
  [7, 9, 12, "Normal"], [8, 10, 16, "Heavy Traffic"], [2, 6, 18, "Normal"], [9, 1, 20, "Normal"],
];

const FUEL_PER_KM = 0.12;
const FUEL_PRICE = 1.5;
const SPEED_KMH = 40;
const MAX_LOCATIONS = 50;

const freshRoads = () =>
  DEFAULT_ROADS.map(([a, b, d, status], i) => ({ id: i, a, b, d, status, open: true }));

let roads = freshRoads();

/* schematic positions: a ring around the city centre */
const CX = 500;
const CY = 350;
const POS = {};
LOCATIONS.forEach((loc, i) => {
  const angle = ((60 - i * 36) * Math.PI) / 180;
  POS[loc.id] = { x: Math.round(CX + 395 * Math.cos(angle)), y: Math.round(CY + 265 * Math.sin(angle)) };
});

const nameOf = (id) => LOCATIONS.find((l) => l.id === id)?.name ?? "Unknown";
const roadBetween = (a, b) => roads.find((r) => (r.a === a && r.b === b) || (r.a === b && r.b === a));
const usable = (r) => r.open && r.status !== "Blocked";
const weightOf = (r) => (r.status === "Heavy Traffic" ? Math.floor(r.d * 1.5) : r.d);

/* ---------- Dijkstra ---------- */

function dijkstra(start, end) {
  const dist = {};
  const parent = {};
  const visited = {};
  LOCATIONS.forEach((l) => {
    dist[l.id] = Infinity;
    parent[l.id] = null;
  });
  dist[start] = 0;

  for (let n = 0; n < LOCATIONS.length; n++) {
    let u = null;
    for (const l of LOCATIONS) {
      if (!visited[l.id] && (u === null || dist[l.id] < dist[u])) u = l.id;
    }
    if (u === null || dist[u] === Infinity) break;
    visited[u] = true;
    for (const r of roads) {
      if (!usable(r) || (r.a !== u && r.b !== u)) continue;
      const v = r.a === u ? r.b : r.a;
      const w = weightOf(r);
      if (!visited[v] && dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;
        parent[v] = u;
      }
    }
  }

  if (dist[end] === Infinity) return null;
  const path = [];
  for (let at = end; at !== null; at = parent[at]) path.unshift(at);
  return { path, weighted: dist[end] };
}

function estimate(weighted) {
  const fuel = weighted * FUEL_PER_KM;
  const time = weighted / SPEED_KMH;
  const hours = Math.floor(time);
  const minutes = Math.floor((time - hours) * 60 + 1e-9);
  return { fuel, hours, minutes, cost: fuel * FUEL_PRICE };
}

/* ---------- linked list (route history) and stack (route builder) ---------- */

class HistoryList {
  constructor() {
    this.head = null;
    this.size = 0;
  }
  add(entry) {
    this.head = { entry, next: this.head };
    this.size++;
  }
  clear() {
    this.head = null;
    this.size = 0;
  }
  toArray() {
    const out = [];
    for (let node = this.head; node; node = node.next) out.push(node.entry);
    return out;
  }
}

class Stack {
  constructor() {
    this.items = [];
  }
  push(x) {
    this.items.push(x);
  }
  pop() {
    return this.items.pop();
  }
  peek() {
    return this.items[this.items.length - 1];
  }
  get size() {
    return this.items.length;
  }
  clear() {
    this.items = [];
  }
}

const history = new HistoryList();
const built = new Stack();

/* ---------- persistence (best effort) ---------- */

const STORE = "lahore-route-planner";
function save() {
  try {
    localStorage.setItem(
      STORE,
      JSON.stringify({ roads: roads.map(({ status, open }) => ({ status, open })), history: history.toArray().reverse() })
    );
  } catch {}
}
function load() {
  try {
    const data = JSON.parse(localStorage.getItem(STORE) || "null");
    if (!data) return;
    if (Array.isArray(data.roads) && data.roads.length === roads.length) {
      data.roads.forEach((s, i) => {
        if (["Normal", "Heavy Traffic", "Blocked"].includes(s.status)) roads[i].status = s.status;
        roads[i].open = s.open !== false;
      });
    }
    if (Array.isArray(data.history)) data.history.forEach((h) => history.add(h));
  } catch {}
}

/* ---------- helpers ---------- */

const $ = (sel) => document.querySelector(sel);
const SVG = "http://www.w3.org/2000/svg";
function el(tag, attrs = {}, parent) {
  const node = document.createElementNS(SVG, tag);
  for (const [k, v] of Object.entries(attrs)) node.setAttribute(k, v);
  if (parent) parent.appendChild(node);
  return node;
}
const esc = (s) => String(s).replace(/[&<>"]/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;" })[c]);
const statusClass = (r) => (!r.open ? "closed" : r.status === "Heavy Traffic" ? "heavy" : r.status === "Blocked" ? "blocked" : "normal");

/* consecutive ring roads bow outward; cross-town roads run straight */
function geometry(r, fromId = r.a) {
  const toId = fromId === r.a ? r.b : r.a;
  const p = POS[fromId];
  const q = POS[toId];
  const gap = Math.abs(r.a - r.b);
  const ring = gap === 1 || gap === LOCATIONS.length - 1;
  const mx = (p.x + q.x) / 2;
  const my = (p.y + q.y) / 2;
  let cx = mx;
  let cy = my;
  if (ring) {
    const ox = mx - CX;
    const oy = my - CY;
    const len = Math.hypot(ox, oy) || 1;
    const bow = Math.hypot(q.x - p.x, q.y - p.y) * 0.14;
    cx = mx + (ox / len) * bow;
    cy = my + (oy / len) * bow;
  }
  return {
    d: `M ${p.x} ${p.y} Q ${cx.toFixed(1)} ${cy.toFixed(1)} ${q.x} ${q.y}`,
    mid: { x: 0.25 * p.x + 0.5 * cx + 0.25 * q.x, y: 0.25 * p.y + 0.5 * cy + 0.25 * q.y },
  };
}

/* ---------- state ---------- */

let currentRoute = null;
let selectedRoad = null;
let clickStage = 0;

/* ---------- map ---------- */

function drawMap() {
  const roadsG = $("#roads");
  const labelsG = $("#labels");
  const nodesG = $("#nodes");
  roadsG.replaceChildren();
  labelsG.replaceChildren();
  nodesG.replaceChildren();
  const svgBox = $("#map").getBoundingClientRect();
  const k = svgBox.width && svgBox.width < 640 ? 1.8 : 1;

  for (const r of roads) {
    const g = geometry(r);
    const cls = `road ${statusClass(r)}${selectedRoad === r.id ? " selected" : ""}`;
    el("path", { d: g.d, class: cls }, roadsG);
    const hit = el("path", { d: g.d, class: "road-hit" }, roadsG);
    const t = el("title", {}, hit);
    t.textContent = `${nameOf(r.a)} – ${nameOf(r.b)}: ${r.d} km, ${r.open ? r.status : "Closed"}`;
    hit.addEventListener("click", () => selectRoad(r.id, true));

    const label = `${r.d}`;
    const w = (label.length * 8.5 + 12) * k;
    el("rect", { x: g.mid.x - w / 2, y: g.mid.y - 10 * k, width: w, height: 20 * k, rx: 10 * k, class: "dist-bg" }, labelsG);
    const txt = el("text", { x: g.mid.x, y: g.mid.y + 4.5 * k, "text-anchor": "middle", class: "dist", style: `font-size:${13 * k}px` }, labelsG);
    txt.textContent = label;
  }

  const onRoute = new Set(currentRoute?.path ?? []);
  const start = currentRoute?.path[0] ?? Number($("#from").value);
  const end = currentRoute ? currentRoute.path[currentRoute.path.length - 1] : null;

  for (const loc of LOCATIONS) {
    const p = POS[loc.id];
    let cls = "node";
    if (onRoute.has(loc.id)) cls += " on-route";
    if (loc.id === start && (currentRoute || clickStage === 1)) cls += " start";
    if (loc.id === end) cls += " end";
    const g = el("g", { class: cls, tabindex: 0, role: "button", "aria-label": `${loc.name}. Select as start or destination` }, nodesG);
    el("circle", { cx: p.x, cy: p.y, r: 19, class: "ring", fill: "none" }, g);
    el("circle", { cx: p.x, cy: p.y, r: 11, class: "dot" }, g);

    const above = p.y < CY;
    const w = (loc.name.length * 8.9 + 20) * k;
    const lh = 26 * k;
    const ly = above ? p.y - 22 - lh : p.y + 22;
    let lx = p.x - w / 2;
    lx = Math.max(4, Math.min(1000 - w - 4, lx));
    el("rect", { x: lx, y: ly, width: w, height: lh, class: "name-bg" }, g);
    const name = el("text", { x: lx + w / 2, y: ly + 18 * k, "text-anchor": "middle", class: "name", style: `font-size:${15 * k}px` }, g);
    name.textContent = loc.name;

    let tag = "";
    if (currentRoute && loc.id === start) tag = "START";
    else if (currentRoute && loc.id === end) tag = "END";
    else if (!currentRoute && clickStage === 1 && loc.id === start) tag = "START";
    if (tag) {
      const tt = el("text", { x: lx + w / 2, y: above ? ly - 6 : ly + lh + 16 * k, "text-anchor": "middle", class: "tag", style: `font-size:${12 * k}px` }, g);
      tt.textContent = tag;
    }

    g.addEventListener("click", () => nodeClicked(loc.id));
    g.addEventListener("keydown", (e) => {
      if (e.key === "Enter" || e.key === " ") {
        e.preventDefault();
        nodeClicked(loc.id);
      }
    });
  }
}

function drawRouteLine(path, animate) {
  const layer = $("#route-layer");
  layer.replaceChildren();
  if (!path) return;
  const parts = [];
  for (let i = 0; i < path.length - 1; i++) {
    const r = roadBetween(path[i], path[i + 1]);
    const d = geometry(r, path[i]).d;
    parts.push(i === 0 ? d : d.replace(/^M [\d.]+ [\d.]+ /, ""));
  }
  const line = el("path", { d: parts.join(" "), class: "route-line" }, layer);
  if (animate && !matchMedia("(prefers-reduced-motion: reduce)").matches) {
    const len = line.getTotalLength();
    line.style.setProperty("--len", len);
    line.style.strokeDasharray = len;
    line.classList.add("draw");
  }
}

/* ---------- route tab ---------- */

function fillSelect(select, value) {
  select.replaceChildren(...LOCATIONS.map((l) => new Option(l.name, l.id)));
  if (value) select.value = value;
}

function legText(a, b) {
  const r = roadBetween(a, b);
  if (r.status === "Heavy Traffic") {
    return `${r.d} km road · <span class="heavy">heavy traffic, counts as ${weightOf(r)} km</span>`;
  }
  return `${r.d} km road`;
}

function stopsHtml(path, topTag = false) {
  return `<ol class="stops">${path
    .map((id, i) => {
      const leg = i < path.length - 1 ? `<span class="leg">${legText(id, path[i + 1])}</span>` : "";
      const top = topTag && i === path.length - 1 ? `<span class="top-tag">TOP</span>` : "";
      return `<li>${esc(nameOf(id))}${top}${leg}</li>`;
    })
    .join("")}</ol>`;
}

function costsHtml(weighted) {
  const e = estimate(weighted);
  return `<dl class="costs">
    <div><dt>Fuel</dt><dd>${e.fuel.toFixed(2)} L</dd></div>
    <div><dt>Time</dt><dd>${e.hours}h ${e.minutes}m</dd></div>
    <div><dt>Cost</dt><dd>$${e.cost.toFixed(2)}</dd></div>
  </dl>`;
}

function findRoute(animate = true, record = true) {
  const from = Number($("#from").value);
  const to = Number($("#to").value);
  const out = $("#route-result");
  clickStage = 0;

  if (from === to) {
    currentRoute = null;
    out.innerHTML = `<div class="result warn"><div class="result-head"><div class="big">Same place</div><p>Pick a different destination.</p></div></div>`;
    drawRouteLine(null);
    drawMap();
    return;
  }

  const res = dijkstra(from, to);
  if (!res) {
    currentRoute = null;
    out.innerHTML = `<div class="result warn"><div class="result-head"><div class="big">No route</div><p>Every road from ${esc(nameOf(from))} to ${esc(nameOf(to))} is blocked or closed. Open a road in the Traffic tab.</p></div></div>`;
    drawRouteLine(null);
    drawMap();
    return;
  }

  currentRoute = res;
  const actual = res.path.slice(1).reduce((sum, id, i) => sum + roadBetween(res.path[i], id).d, 0);
  const roadsUsed = res.path.length - 1;
  out.innerHTML = `<div class="result">
    <div class="result-head">
      <div class="big">${res.weighted}<small>km weighted</small></div>
      <p>${esc(nameOf(from))} → ${esc(nameOf(to))} · ${roadsUsed} road${roadsUsed === 1 ? "" : "s"}</p>
    </div>
    ${stopsHtml(res.path)}
    ${costsHtml(res.weighted)}
    <div class="result-foot"><span>Actual road length ${actual} km</span><span>${SPEED_KMH} km/h · $${FUEL_PRICE.toFixed(2)}/L</span></div>
  </div>`;

  drawMap();
  drawRouteLine(res.path, animate);

  if (record) {
    history.add({ path: res.path, weighted: res.weighted, kind: "Fastest", at: Date.now() });
    renderHistory();
    save();
  }
}

function nodeClicked(id) {
  if (clickStage === 0) {
    $("#from").value = id;
    clickStage = 1;
    currentRoute = null;
    drawRouteLine(null);
    $("#route-result").innerHTML = `<p class="hint">Start set to <strong>${esc(nameOf(id))}</strong>. Now click the destination.</p>`;
    showTab("route");
    drawMap();
  } else {
    $("#to").value = id;
    findRoute();
  }
}

/* ---------- build tab ---------- */

let buildError = "";

function renderBuild() {
  const out = $("#build-result");
  const path = built.items;
  let html = `<div class="stack">`;
  if (!path.length) {
    html += `<p class="stack-empty">No stops yet. Add your starting point.</p>`;
  } else {
    html += stopsHtml(path, true);
  }
  html += `</div>`;
  if (path.length > 1) {
    const weighted = path.slice(1).reduce((s, id, i) => s + weightOf(roadBetween(path[i], id)), 0);
    html += `<p class="build-total">Weighted distance <strong>${weighted} km</strong></p>`;
    html += `<div class="result">${costsHtml(weighted)}</div>`;
  }
  if (buildError) html += `<p class="error-text" role="alert">${esc(buildError)}</p>`;
  out.innerHTML = html;

  $("#build-pop").disabled = !path.length;
  $("#build-clear").disabled = !path.length;
  $("#build-save").disabled = path.length < 2;

  if (document.querySelector("#view-build:not([hidden])")) {
    currentRoute = path.length ? { path: [...path], weighted: 0 } : null;
    drawMap();
    drawRouteLine(path.length > 1 ? path : null, false);
  }
}

function pushStop() {
  const id = Number($("#build-add").value);
  const top = built.peek();
  buildError = "";
  if (top === id) {
    buildError = `You're already at ${nameOf(id)}.`;
  } else if (top !== undefined) {
    const r = roadBetween(top, id);
    if (!r) buildError = `There's no direct road from ${nameOf(top)} to ${nameOf(id)}.`;
    else if (!usable(r)) buildError = `The road from ${nameOf(top)} to ${nameOf(id)} is ${r.open ? "blocked" : "closed"}.`;
  }
  if (!buildError) built.push(id);
  renderBuild();
  suggestNext();
}

function suggestNext() {
  const top = built.peek();
  if (top === undefined) return;
  const next = roads.filter((r) => usable(r) && (r.a === top || r.b === top)).map((r) => (r.a === top ? r.b : r.a));
  if (next.length) $("#build-add").value = next[0];
}

/* ---------- traffic tab ---------- */

function renderRoads() {
  const list = $("#road-list");
  list.replaceChildren();
  for (const r of roads) {
    const li = document.createElement("li");
    li.dataset.id = r.id;
    if (selectedRoad === r.id) li.classList.add("selected");
    const label = `${nameOf(r.a)} – ${nameOf(r.b)}`;
    li.innerHTML = `
      <div class="road-name">${esc(label)}<small>${r.d} km${r.status === "Heavy Traffic" ? ` · counts as ${weightOf(r)} km` : ""}</small></div>
      <div class="seg" role="group" aria-label="Traffic on ${esc(label)}">
        ${["Normal", "Heavy Traffic", "Blocked"]
          .map((s) => `<button type="button" data-v="${s}" aria-pressed="${r.status === s}">${s === "Heavy Traffic" ? "Heavy" : s}</button>`)
          .join("")}
      </div>
      <label class="open-toggle"><input type="checkbox" ${r.open ? "checked" : ""} /> Road open</label>`;
    li.querySelectorAll(".seg button").forEach((b) =>
      b.addEventListener("click", () => {
        r.status = b.dataset.v;
        selectedRoad = r.id;
        trafficChanged();
      })
    );
    li.querySelector("input").addEventListener("change", (e) => {
      r.open = e.target.checked;
      selectedRoad = r.id;
      trafficChanged();
    });
    list.appendChild(li);
  }
}

function trafficChanged() {
  save();
  renderRoads();
  renderStats();
  if (currentRoute && document.querySelector("#view-route:not([hidden])")) findRoute(false, false);
  else drawMap();
}

function selectRoad(id, fromMap) {
  selectedRoad = id;
  if (fromMap) showTab("traffic");
  renderRoads();
  drawMap();
  const li = document.querySelector(`#road-list li[data-id="${id}"]`);
  if (li && fromMap) {
    li.scrollIntoView({ block: "nearest", behavior: "smooth" });
    li.querySelector("button[aria-pressed='true']")?.focus({ preventScroll: true });
  }
}

/* ---------- history tab ---------- */

function renderHistory() {
  const list = $("#history-list");
  const items = history.toArray();
  if (!items.length) {
    list.innerHTML = `<li class="empty">No routes yet. Find one in the Route tab.</li>`;
  } else {
    list.innerHTML = items
      .map((h) => {
        const e = estimate(h.weighted);
        const when = new Date(h.at).toLocaleTimeString([], { hour: "2-digit", minute: "2-digit" });
        return `<li>
          <div class="h-path">${h.path.map((id) => esc(nameOf(id))).join(" → ")}</div>
          <div class="h-meta">${esc(h.kind)} · <b>${h.weighted} km</b> · ${e.hours}h ${e.minutes}m · $${e.cost.toFixed(2)} · ${when}</div>
        </li>`;
      })
      .join("");
  }
  $("#clear-history").disabled = !items.length;
  renderStats();
}

/* ---------- stats tab ---------- */

function selectionSort(arr) {
  const a = [...arr];
  for (let i = 0; i < a.length - 1; i++) {
    let min = i;
    for (let j = i + 1; j < a.length; j++) if (a[j].localeCompare(a[min]) < 0) min = j;
    [a[i], a[min]] = [a[min], a[i]];
  }
  return a;
}

function renderStats() {
  const count = (fn) => roads.filter(fn).length;
  const avg = roads.reduce((s, r) => s + r.d, 0) / roads.length;
  const items = [
    ["Locations", `${LOCATIONS.length}<small style="font-size:14px;color:var(--muted)"> / ${MAX_LOCATIONS}</small>`],
    ["Roads", `${roads.length}`],
    ["Normal", count((r) => r.open && r.status === "Normal")],
    ["Heavy traffic", count((r) => r.open && r.status === "Heavy Traffic")],
    ["Blocked", count((r) => r.open && r.status === "Blocked")],
    ["Closed", count((r) => !r.open)],
    ["Average road", `${avg.toFixed(2)} km`],
    ["Saved routes", history.size],
  ];
  $("#stats").innerHTML = items.map(([k, v]) => `<div><dt>${k}</dt><dd>${v}</dd></div>`).join("");
  $("#sorted-locations").innerHTML = selectionSort(LOCATIONS.map((l) => l.name))
    .map((n) => `<li>${esc(n)}</li>`)
    .join("");
}

/* ---------- tabs ---------- */

const TABS = ["route", "build", "traffic", "history", "stats"];

function showTab(name) {
  for (const t of TABS) {
    const on = t === name;
    $(`#tab-${t}`).setAttribute("aria-selected", on);
    $(`#tab-${t}`).tabIndex = on ? 0 : -1;
    $(`#view-${t}`).hidden = !on;
  }
  if (name === "build") {
    renderBuild();
  } else if (name === "route") {
    if (currentRoute && currentRoute.weighted === 0) {
      currentRoute = null;
      drawRouteLine(null);
    }
    if (currentRoute) {
      drawMap();
      drawRouteLine(currentRoute.path, false);
    } else drawMap();
  } else {
    if (currentRoute && currentRoute.weighted === 0) {
      currentRoute = null;
      drawRouteLine(null);
    }
    drawMap();
  }
}

/* ---------- wire up ---------- */

function init() {
  load();
  fillSelect($("#from"), 1);
  fillSelect($("#to"), 7);
  fillSelect($("#build-add"), 1);

  TABS.forEach((t, i) => {
    const btn = $(`#tab-${t}`);
    btn.addEventListener("click", () => showTab(t));
    btn.addEventListener("keydown", (e) => {
      let j = null;
      if (e.key === "ArrowRight") j = (i + 1) % TABS.length;
      if (e.key === "ArrowLeft") j = (i - 1 + TABS.length) % TABS.length;
      if (e.key === "Home") j = 0;
      if (e.key === "End") j = TABS.length - 1;
      if (j !== null) {
        e.preventDefault();
        showTab(TABS[j]);
        $(`#tab-${TABS[j]}`).focus();
      }
    });
  });

  $("#route-form").addEventListener("submit", (e) => {
    e.preventDefault();
    findRoute();
  });
  $("#swap").addEventListener("click", () => {
    const f = $("#from").value;
    $("#from").value = $("#to").value;
    $("#to").value = f;
    if (currentRoute) findRoute(true, false);
  });
  $("#from").addEventListener("change", () => {
    clickStage = 0;
    if (!currentRoute) drawMap();
  });

  $("#build-push").addEventListener("click", pushStop);
  $("#build-pop").addEventListener("click", () => {
    buildError = "";
    built.pop();
    renderBuild();
  });
  $("#build-clear").addEventListener("click", () => {
    buildError = "";
    built.clear();
    renderBuild();
  });
  $("#build-save").addEventListener("click", () => {
    const path = [...built.items];
    const weighted = path.slice(1).reduce((s, id, i) => s + weightOf(roadBetween(path[i], id)), 0);
    history.add({ path, weighted, kind: "Custom", at: Date.now() });
    save();
    renderHistory();
    buildError = "";
    $("#build-result").insertAdjacentHTML("beforeend", `<p class="hint">Saved to history.</p>`);
  });

  $("#reset-traffic").addEventListener("click", () => {
    roads = freshRoads();
    selectedRoad = null;
    trafficChanged();
  });
  $("#clear-history").addEventListener("click", () => {
    history.clear();
    save();
    renderHistory();
  });

  renderRoads();
  renderHistory();
  showTab("route");
  findRoute(true, false);

  let wasCompact = $("#map").getBoundingClientRect().width < 640;
  addEventListener("resize", () => {
    const compact = $("#map").getBoundingClientRect().width < 640;
    if (compact !== wasCompact) {
      wasCompact = compact;
      drawMap();
    }
  });
}

init();
