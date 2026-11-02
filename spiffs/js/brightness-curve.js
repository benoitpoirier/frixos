/* ============================================================
   brightness-curve.js — low-overhead adaptive dimming editor.
   Uses one SVG scene, reuses all nodes, and only polls while the
   Display settings are visible in brightness-based mode.
   ============================================================ */

(() => {
  const SVG_NS = 'http://www.w3.org/2000/svg';
  const WIDTH_FALLBACK = 360;
  const CHART_HEIGHT = 220;
  const HISTORY_HEIGHT = 192;
  const MIN_LUX = 0.1;
  const MAX_LUX = 6553;
  const MIN_LEVEL = 1;
  const MAX_LEVEL = 100;
  const MIN_LOG_GAP = 0.1;
  const MIN_LOG_GAP_RATIO = Math.pow(10, MIN_LOG_GAP);
  const DEFAULT_THRESHOLD = 2;
  const DEFAULT_SENSITIVITY = 0.5;
  const MAX_THRESHOLD_LUX = 500;
  const MAX_SENSITIVITY_LUX = 50;
  const LUX_STEP = 0.1;
  const COARSE_LUX_STEP = 1;
  const DEFAULT_POINTS = [
    { lux: 1.2, pct: 2 },
    { lux: 3.2, pct: 44 },
    { lux: 31, pct: 55 },
    { lux: 130, pct: 76 },
  ];
  const POLL_INTERVAL_MS = 10000;
  const TOUCH_SLOP = 4;
  const HANDLE_HIT_RADIUS = 22;
  const THRESHOLD_HANDLE_OFFSET_Y = 1;
  const SENSITIVITY_HANDLE_OFFSET_Y = 34;
  const EMPTY_SAMPLE = -1;
  const POINT_COUNT = 4;
  const HISTORY_HOURS = 24;
  const MORPH_DURATION_MS = 220;

  const chart = el('bcChart');
  const historyChart = el('bcHist');
  const wrapper = el('bcWrap');
  const dimBright = el('dimBright');
  const dimMode = el('dim_mode');
  const thresholdError = el('bcThresholdError');
  const statusNode = el('bcStatus');

  if (!chart || !historyChart || !wrapper || !dimBright || !dimMode ||
      !thresholdError)
    return;

  const state = {
    points: DEFAULT_POINTS.map(point => ({ ...point })),
    threshold: DEFAULT_THRESHOLD,
    sensitivity: DEFAULT_SENSITIVITY,
    liveLux: null,
    yesterday: null,
    today: null,
    hour: -1,
    historyLoaded: false,
    ready: false,
    animating: false,
    animationFrame: null,
    domain: { min: MIN_LUX, max: 1000 },
    drag: null,
    hoverPoint: -1,
    pollTimer: null,
    requestBusy: false,
    pendingHistory: false,
    historyError: false,
    liveError: false,
  };
  state.displayPoints = state.points.map(point => ({ ...point }));

  let viewWidth = WIDTH_FALLBACK;
  let resizeObserver = null;
  let activePanelObserver = null;
  const changeListeners = [];
  const pointRefs = [];
  const historyRefs = { today: [], yesterday: [] };
  const chartRefs = {};

  function tr(key, vars) {
    const path = 'advanced.brightness.curve.' + key;
    const active = getNestedTranslation(translations[currentLanguage] || translations.en, path);
    const fallback = getNestedTranslation(translations.en, path);
    let value = active || fallback || key;
    Object.keys(vars || {}).forEach(name => {
      value = value.replaceAll('{' + name + '}', String(vars[name]));
    });
    return value;
  }

  function stopCurveAnimation() {
    if (state.animationFrame !== null)
      cancelAnimationFrame(state.animationFrame);
    state.animationFrame = null;
    state.animating = false;
  }

  function animateCurveTo(target) {
    stopCurveAnimation();
    const from = state.displayPoints.map(point => ({ ...point }));
    const to = target.map(point => ({ ...point }));
    const reduceMotion = typeof window.matchMedia === 'function' &&
      window.matchMedia('(prefers-reduced-motion: reduce)').matches;
    if (typeof ResizeObserver !== 'function' || reduceMotion) {
      state.displayPoints = to;
      render();
      return;
    }

    const started = Date.now();
    state.animating = true;
    const frame = () => {
      const progress = Math.min(1, (Date.now() - started) / MORPH_DURATION_MS);
      const eased = 1 - Math.pow(1 - progress, 3);
      state.displayPoints = to.map((point, index) => ({
        lux: Math.pow(10,
          Math.log10(from[index].lux) +
          (Math.log10(point.lux) - Math.log10(from[index].lux)) * eased),
        pct: from[index].pct + (point.pct - from[index].pct) * eased,
      }));
      render();
      if (progress < 1) {
        state.animationFrame = requestAnimationFrame(frame);
      } else {
        state.displayPoints = to;
        state.animationFrame = null;
        state.animating = false;
        render();
      }
    };
    state.animationFrame = requestAnimationFrame(frame);
  }

  function number(value, min, max) {
    const parsed = Number(value);
    return Number.isFinite(parsed) && parsed >= min && parsed <= max ? parsed : null;
  }

  function roundTenth(value) {
    return Math.round(Number(value) * 10) / 10;
  }

  function formatLux(value) {
    const amount = Number(value);
    if (!Number.isFinite(amount)) return '—';
    if (amount >= 1000) {
      const thousands = amount / 1000;
      return (Number.isInteger(thousands) ? String(thousands) : thousands.toFixed(1)) + 'k';
    }
    return new Intl.NumberFormat(currentLanguage || 'en', {
      maximumFractionDigits: 1,
      minimumFractionDigits: 0,
    }).format(amount);
  }

  function svgNode(parent, tag, className, attrs) {
    const node = document.createElementNS(SVG_NS, tag);
    if (className) node.setAttribute('class', className);
    Object.keys(attrs || {}).forEach(key => node.setAttribute(key, String(attrs[key])));
    if (parent) parent.appendChild(node);
    return node;
  }

  function svgText(parent, className, attrs, value) {
    const node = svgNode(parent, 'text', className, attrs);
    node.textContent = value || '';
    return node;
  }

  function setText(node, value) {
    if (node && node.textContent !== value) node.textContent = value;
  }

  function setAttr(node, name, value) {
    if (!node) return;
    const next = String(value);
    if (node.getAttribute(name) !== next) node.setAttribute(name, next);
  }

  function createChartScene() {
    const defs = svgNode(chart, 'defs');
    const gradient = svgNode(defs, 'linearGradient', null, {
      id: 'bcAreaGradient',
      x1: '0',
      x2: '0',
      y1: '0',
      y2: '1',
    });
    svgNode(gradient, 'stop', 'bc-gradient-start', { offset: '0%' });
    svgNode(gradient, 'stop', 'bc-gradient-end', { offset: '100%' });
    const pointGradient = svgNode(defs, 'radialGradient', null, {
      id: 'bcPointGradient',
      cx: '35%',
      cy: '25%',
      r: '80%',
    });
    svgNode(pointGradient, 'stop', 'bc-point-gradient-start', { offset: '0%' });
    svgNode(pointGradient, 'stop', 'bc-point-gradient-end', { offset: '100%' });

    chartRefs.background = svgNode(chart, 'rect', 'bc-bg', { rx: 9 });
    chartRefs.band = svgNode(chart, 'rect', 'bc-band');
    chartRefs.bandLeft = svgNode(chart, 'line', 'bc-band-edge');
    chartRefs.bandRight = svgNode(chart, 'line', 'bc-band-edge');

    chartRefs.minorGrid = [];
    for (let i = 0; i < 64; i++)
      chartRefs.minorGrid.push(svgNode(chart, 'line', 'bc-grid-minor'));

    chartRefs.majorGrid = [];
    chartRefs.majorLabels = [];
    for (let i = 0; i < 8; i++) {
      chartRefs.majorGrid.push(svgNode(chart, 'line', 'bc-grid'));
      chartRefs.majorLabels.push(svgText(chart, 'bc-axis-label', {
        'text-anchor': 'middle',
      }, ''));
    }

    chartRefs.levelGrid = [];
    chartRefs.levelLabels = [];
    for (const pct of [0, 25, 50, 75, 100]) {
      chartRefs.levelGrid.push(svgNode(chart, 'line', 'bc-grid'));
      chartRefs.levelLabels.push(svgText(chart, 'bc-axis-label', {
        'text-anchor': 'end',
      }, String(pct)));
    }

    chartRefs.axisLux = svgText(chart, 'bc-axis-title', { 'text-anchor': 'end' }, '');
    chartRefs.axisPercent = svgText(chart, 'bc-axis-title', { 'text-anchor': 'start' }, '');
    chartRefs.area = svgNode(chart, 'path', 'bc-area');
    chartRefs.extensionLeft = svgNode(chart, 'path', 'bc-curve-extension');
    chartRefs.curve = svgNode(chart, 'path', 'bc-curve');
    chartRefs.extensionRight = svgNode(chart, 'path', 'bc-curve-extension');

    chartRefs.thresholdLine = svgNode(chart, 'line', 'bc-threshold-line');
    chartRefs.rugToday = [];
    chartRefs.rugYesterday = [];
    for (let i = 0; i < 48; i++) {
      chartRefs.rugYesterday.push(svgNode(chart, 'line', 'bc-rug bc-rug-yesterday'));
      chartRefs.rugToday.push(svgNode(chart, 'line', 'bc-rug bc-rug-today'));
    }

    chartRefs.liveLine = svgNode(chart, 'line', 'bc-live-line');
    chartRefs.livePulse = svgNode(chart, 'circle', 'bc-live-pulse', { r: 5 });
    chartRefs.livePoint = svgNode(chart, 'circle', 'bc-live-point', { r: 4 });
    chartRefs.liveLabelGroup = svgNode(chart, 'g', 'bc-live-label-group');
    chartRefs.liveLabelBg = svgNode(chartRefs.liveLabelGroup, 'rect', 'bc-live-label-bg', { rx: 9, height: 20 });
    chartRefs.liveLabel = svgText(chartRefs.liveLabelGroup, 'bc-live-label', { y: 13 }, '');
    chartRefs.liveOverflow = svgText(chart, 'bc-live-overflow', { 'text-anchor': 'middle' }, '');

    chartRefs.thresholdGroup = svgNode(chart, 'g', 'bc-handle-group', {
      tabindex: '0',
      role: 'slider',
      'aria-valuemin': '0',
      'aria-valuemax': String(MAX_THRESHOLD_LUX),
    });
    chartRefs.thresholdHandle = svgNode(chartRefs.thresholdGroup, 'rect', 'bc-handle', {
      x: -12, y: -8, width: 24, height: 16, rx: 6,
    });
    svgNode(chartRefs.thresholdGroup, 'path', 'bc-handle-icon', {
      d: 'M-6 -2a3 3 0 1 0 3 4a3 3 0 0 1-3-4M4 -4v2m0 4v2m-3-3h2m2 0h2',
    });
    svgNode(chartRefs.thresholdGroup, 'circle', 'bc-handle-target', { cx: 0, cy: 0, r: HANDLE_HIT_RADIUS });

    chartRefs.lowerGroup = svgNode(chart, 'g', 'bc-handle-group', {
      tabindex: '0',
      role: 'slider',
      'aria-valuemin': '0',
      'aria-valuemax': '50',
    });
    svgNode(chartRefs.lowerGroup, 'rect', 'bc-handle', { x: -2, y: -7, width: 4, height: 14, rx: 2 });
    svgNode(chartRefs.lowerGroup, 'rect', 'bc-handle-target', {
      x: -HANDLE_HIT_RADIUS, y: -HANDLE_HIT_RADIUS,
      width: HANDLE_HIT_RADIUS * 2, height: HANDLE_HIT_RADIUS * 2,
    });

    chartRefs.upperGroup = svgNode(chart, 'g', 'bc-handle-group', {
      tabindex: '0',
      role: 'slider',
      'aria-valuemin': '0',
      'aria-valuemax': '50',
    });
    svgNode(chartRefs.upperGroup, 'rect', 'bc-handle', { x: -2, y: -7, width: 4, height: 14, rx: 2 });
    svgNode(chartRefs.upperGroup, 'rect', 'bc-handle-target', {
      x: -HANDLE_HIT_RADIUS, y: -HANDLE_HIT_RADIUS,
      width: HANDLE_HIT_RADIUS * 2, height: HANDLE_HIT_RADIUS * 2,
    });

    for (let i = 0; i < POINT_COUNT; i++) {
      const group = svgNode(chart, 'g', 'bc-point-group', {
        tabindex: '0',
        role: 'slider',
        'aria-valuemin': String(MIN_LEVEL),
        'aria-valuemax': String(MAX_LEVEL),
      });
      const hit = svgNode(group, 'circle', 'bc-target', { r: 23 });
      const halo = svgNode(group, 'circle', 'bc-point-halo', { r: 14 });
      const disc = svgNode(group, 'circle', 'bc-point-disc', { r: 7 });
      pointRefs.push({ group, hit, halo, disc });
    }

    chartRefs.historyLegendLabel = svgText(historyChart, 'bc-hist-title', { x: 5, y: 12 }, '');
    chartRefs.yesterdayLegendSwatch = svgNode(historyChart, 'line', 'bc-history-legend-yesterday', {
      x1: 210, x2: 218, y1: 9, y2: 9,
    });
    chartRefs.yesterdayLegend = svgText(historyChart, 'bc-legend', { x: 220, y: 12 }, '');
    chartRefs.todayLegendSwatch = svgNode(historyChart, 'line', 'bc-history-legend-today', {
      x1: 270, x2: 278, y1: 9, y2: 9,
    });
    chartRefs.todayLegend = svgText(historyChart, 'bc-legend', { x: 280, y: 12 }, '');
    chartRefs.historyBand = svgNode(historyChart, 'rect', 'bc-hist-band');
    chartRefs.historyCurrent = svgNode(historyChart, 'rect', 'bc-current-hour', { rx: 4 });
    chartRefs.historyThreshold = svgNode(historyChart, 'line', 'bc-hist-threshold');
    chartRefs.historyBaseline = svgNode(historyChart, 'line', 'bc-hist-baseline');
    chartRefs.historyTodayArea = svgNode(historyChart, 'path', 'bc-history-area-today');
    chartRefs.historyYesterdayCurve = svgNode(historyChart, 'path', 'bc-history-curve-yesterday');
    chartRefs.historyTodayCurve = svgNode(historyChart, 'path', 'bc-history-curve-today');
    chartRefs.historyLuxLabels = [
      svgText(historyChart, 'bc-axis-label', { 'text-anchor': 'end' }, ''),
      svgText(historyChart, 'bc-axis-label', { 'text-anchor': 'end' }, ''),
      svgText(historyChart, 'bc-axis-label', { 'text-anchor': 'end' }, ''),
    ];
    chartRefs.historyYesterdayPoints = [];
    chartRefs.historyTodayPoints = [];
    chartRefs.historyEmpty = [];
    for (let i = 0; i < HISTORY_HOURS; i++) {
      const yesterdayPoint = svgNode(historyChart, 'circle', 'bc-history-point bc-history-point-yesterday', { r: 2.5 });
      const todayPoint = svgNode(historyChart, 'circle', 'bc-history-point bc-history-point-today', { r: 2.5 });
      historyRefs.yesterday.push(yesterdayPoint);
      historyRefs.today.push(todayPoint);
      chartRefs.historyYesterdayPoints.push(yesterdayPoint);
      chartRefs.historyTodayPoints.push(todayPoint);
      chartRefs.historyEmpty.push(svgNode(historyChart, 'circle', 'bc-empty', { r: 1.5 }));
    }
    chartRefs.historyTicks = [];
    chartRefs.historyTickLabels = [];
    for (const tick of [0, 6, 12, 18, 24]) {
      chartRefs.historyTicks.push(svgNode(historyChart, 'line', 'bc-hist-grid'));
      chartRefs.historyTickLabels.push(svgText(historyChart, 'bc-axis-label', {
        'text-anchor': tick === 0 ? 'start' : tick === 24 ? 'end' : 'middle',
      }, String(tick)));
    }
    chartRefs.noReadings = svgText(historyChart, 'bc-no-readings', {
      x: WIDTH_FALLBACK / 2,
      y: HISTORY_HEIGHT / 2 + 4,
    }, '');

    chartRefs.pointGroups = pointRefs.map(ref => ref.group);
    chartRefs.axisLux.setAttribute('x', '0');
    chartRefs.axisLux.setAttribute('y', '0');
    chartRefs.axisPercent.setAttribute('x', '3');
    chartRefs.axisPercent.setAttribute('y', '0');

    updateSize();
  }

  function plotGeometry() {
    const left = 42;
    const right = viewWidth - 14;
    const top = 25;
    const bottom = CHART_HEIGHT - 24;
    return { left, right, top, bottom, width: right - left, height: bottom - top };
  }

  function getSamplesMax() {
    let highest = 0;
    [state.today, state.yesterday].forEach(samples => {
      (samples || []).forEach(value => {
        if (Number.isFinite(value) && value >= 0)
          highest = Math.max(highest, value);
      });
    });
    return highest;
  }

  function computeDomain() {
    const first = state.points[0].lux;
    const last = state.points[POINT_COUNT - 1].lux;
    const lowThreshold = state.threshold - state.sensitivity;
    const minCandidate = Math.min(MIN_LUX, first / 2, lowThreshold > 0 ? lowThreshold : MIN_LUX);
    const maxCandidate = Math.max(1000, last * 2, state.threshold + state.sensitivity,
      getSamplesMax(), state.liveLux || 0);
    const minExp = Math.floor(Math.log10(Math.max(minCandidate, 0.01)));
    const maxExp = Math.ceil(Math.log10(Math.max(maxCandidate, MIN_LUX)));
    return { min: Math.pow(10, minExp), max: Math.pow(10, Math.max(maxExp, minExp + 1)) };
  }

  function activeDomain() {
    return state.drag ? state.drag.domain : computeDomain();
  }

  function xForLux(lux, domain, geometry) {
    const safeLux = Math.max(domain.min, Math.min(domain.max, Number(lux) || domain.min));
    const start = Math.log10(domain.min);
    const span = Math.log10(domain.max) - start;
    return geometry.left + ((Math.log10(safeLux) - start) / span) * geometry.width;
  }

  function luxForX(x, domain, geometry) {
    const ratio = Math.max(0, Math.min(1, (x - geometry.left) / geometry.width));
    return Math.pow(10, Math.log10(domain.min) +
      ratio * (Math.log10(domain.max) - Math.log10(domain.min)));
  }

  function yForPercent(pct, geometry) {
    return geometry.bottom - (Math.max(0, Math.min(100, pct)) / 100) * geometry.height;
  }

  function pointLevelAt(lux) {
    const safeLux = Math.max(MIN_LUX, lux);
    const logLux = Math.log10(safeLux);
    if (safeLux <= state.displayPoints[0].lux) return state.displayPoints[0].pct;
    if (safeLux >= state.displayPoints[POINT_COUNT - 1].lux)
      return state.displayPoints[POINT_COUNT - 1].pct;
    for (let i = 0; i < POINT_COUNT - 1; i++) {
      const a = state.displayPoints[i];
      const b = state.displayPoints[i + 1];
      if (safeLux <= b.lux) {
        const ratio = (logLux - Math.log10(a.lux)) /
          (Math.log10(b.lux) - Math.log10(a.lux));
        return a.pct + (b.pct - a.pct) * ratio;
      }
    }
    return state.points[POINT_COUNT - 1].pct;
  }

  function tickLabel(exponent) {
    if (exponent === 3) return '1k';
    if (exponent > 3) return formatLux(Math.pow(10, exponent));
    return formatLux(Math.pow(10, exponent));
  }

  function renderAxes(domain, geometry) {
    const minExp = Math.floor(Math.log10(domain.min));
    const maxExp = Math.ceil(Math.log10(domain.max));
    const exponentCount = maxExp - minExp + 1;

    for (let i = 0; i < chartRefs.majorGrid.length; i++) {
      const line = chartRefs.majorGrid[i];
      const label = chartRefs.majorLabels[i];
      const exponent = minExp + i;
      const visible = exponent <= maxExp && i < exponentCount;
      line.style.display = visible ? '' : 'none';
      label.style.display = visible ? '' : 'none';
      if (!visible) continue;

      const x = xForLux(Math.pow(10, exponent), domain, geometry);
      setAttr(line, 'x1', x);
      setAttr(line, 'x2', x);
      setAttr(line, 'y1', geometry.top);
      setAttr(line, 'y2', geometry.bottom);
      setAttr(label, 'x', x);
      setAttr(label, 'y', CHART_HEIGHT - 5);
      setText(label, tickLabel(exponent));
      label.setAttribute('text-anchor', i === 0 ? 'start' : exponent === maxExp ? 'end' : 'middle');
      if (viewWidth < 360 && i > 0 && i < exponentCount - 1 && i % 2 !== 0)
        label.style.display = 'none';
    }

    let minorIndex = 0;
    for (let exponent = minExp; exponent < maxExp; exponent++) {
      for (let multiplier = 2; multiplier <= 9; multiplier++) {
        const line = chartRefs.minorGrid[minorIndex++];
        const lux = multiplier * Math.pow(10, exponent);
        const visible = lux > domain.min && lux < domain.max;
        line.style.display = visible ? '' : 'none';
        if (!visible) continue;
        const x = xForLux(lux, domain, geometry);
        setAttr(line, 'x1', x);
        setAttr(line, 'x2', x);
        setAttr(line, 'y1', geometry.top);
        setAttr(line, 'y2', geometry.bottom);
      }
    }
    for (; minorIndex < chartRefs.minorGrid.length; minorIndex++)
      chartRefs.minorGrid[minorIndex].style.display = 'none';

    const levels = [0, 25, 50, 75, 100];
    for (let i = 0; i < levels.length; i++) {
      const y = yForPercent(levels[i], geometry);
      setAttr(chartRefs.levelGrid[i], 'x1', geometry.left);
      setAttr(chartRefs.levelGrid[i], 'x2', geometry.right);
      setAttr(chartRefs.levelGrid[i], 'y1', y);
      setAttr(chartRefs.levelGrid[i], 'y2', y);
      setAttr(chartRefs.levelLabels[i], 'x', geometry.left - 7);
      setAttr(chartRefs.levelLabels[i], 'y', y + 3);
    }

    setAttr(chartRefs.axisLux, 'x', geometry.right);
    setAttr(chartRefs.axisLux, 'y', geometry.top - 8);
    setText(chartRefs.axisLux, tr('axis_lux'));
    setAttr(chartRefs.axisPercent, 'x', geometry.left + 2);
    setAttr(chartRefs.axisPercent, 'y', geometry.top - 8);
    setText(chartRefs.axisPercent, tr('axis_percent'));
  }

  function renderCurve(domain, geometry) {
    const coords = state.displayPoints.map(point => ({
      x: xForLux(point.lux, domain, geometry),
      y: yForPercent(point.pct, geometry),
    }));
    const first = coords[0];
    const last = coords[coords.length - 1];
    const curvePath = coords.map((point, index) =>
      (index === 0 ? 'M' : 'L') + point.x.toFixed(2) + ' ' + point.y.toFixed(2)).join(' ');
    const areaPath = [
      'M' + geometry.left.toFixed(2) + ' ' + first.y.toFixed(2),
      'L' + coords.map(point => point.x.toFixed(2) + ' ' + point.y.toFixed(2)).join(' L'),
      'L' + geometry.right.toFixed(2) + ' ' + last.y.toFixed(2),
      'L' + geometry.right.toFixed(2) + ' ' + geometry.bottom.toFixed(2),
      'L' + geometry.left.toFixed(2) + ' ' + geometry.bottom.toFixed(2) + 'Z',
    ].join(' ');
    setAttr(chartRefs.area, 'd', areaPath);
    setAttr(chartRefs.curve, 'd', curvePath);
    setAttr(chartRefs.extensionLeft, 'd',
      'M' + geometry.left.toFixed(2) + ' ' + first.y.toFixed(2) +
      'L' + first.x.toFixed(2) + ' ' + first.y.toFixed(2));
    setAttr(chartRefs.extensionRight, 'd',
      'M' + last.x.toFixed(2) + ' ' + last.y.toFixed(2) +
      'L' + geometry.right.toFixed(2) + ' ' + last.y.toFixed(2));

    coords.forEach((point, i) => {
      const ref = pointRefs[i];
      setAttr(ref.hit, 'cx', point.x);
      setAttr(ref.hit, 'cy', point.y);
      setAttr(ref.halo, 'cx', point.x);
      setAttr(ref.halo, 'cy', point.y);
      setAttr(ref.disc, 'cx', point.x);
      setAttr(ref.disc, 'cy', point.y);
      const linked = state.hoverPoint === i || (state.drag && state.drag.kind === 'point' && state.drag.index === i);
      ref.group.classList.toggle('is-linked', linked);
      ref.group.classList.toggle('is-active', !!(state.drag && state.drag.kind === 'point' && state.drag.index === i));
      ref.group.setAttribute('aria-valuenow', String(state.points[i].pct));
      const pointLabel = tr('aria_point', {
        point: i + 1,
        lux: formatLux(state.points[i].lux),
        pct: state.points[i].pct,
      });
      ref.group.setAttribute('aria-label', pointLabel);
      ref.group.setAttribute('aria-valuetext', pointLabel);

    });
  }

  function renderThreshold(domain, geometry) {
    const lowerLux = Math.max(domain.min, state.threshold - state.sensitivity);
    const upperLux = Math.max(lowerLux, state.threshold + state.sensitivity);
    const lowerX = xForLux(lowerLux, domain, geometry);
    const centerX = xForLux(Math.max(MIN_LUX, state.threshold), domain, geometry);
    const upperX = xForLux(upperLux, domain, geometry);

    setAttr(chartRefs.band, 'x', lowerX);
    setAttr(chartRefs.band, 'y', geometry.top);
    setAttr(chartRefs.band, 'width', Math.max(0, upperX - lowerX));
    setAttr(chartRefs.band, 'height', geometry.height);
    setAttr(chartRefs.bandLeft, 'x1', lowerX);
    setAttr(chartRefs.bandLeft, 'x2', lowerX);
    setAttr(chartRefs.bandLeft, 'y1', geometry.top);
    setAttr(chartRefs.bandLeft, 'y2', geometry.bottom);
    setAttr(chartRefs.bandRight, 'x1', upperX);
    setAttr(chartRefs.bandRight, 'x2', upperX);
    setAttr(chartRefs.bandRight, 'y1', geometry.top);
    setAttr(chartRefs.bandRight, 'y2', geometry.bottom);
    setAttr(chartRefs.thresholdLine, 'x1', centerX);
    setAttr(chartRefs.thresholdLine, 'x2', centerX);
    setAttr(chartRefs.thresholdLine, 'y1', geometry.top);
    setAttr(chartRefs.thresholdLine, 'y2', geometry.bottom);
    setAttr(chartRefs.thresholdGroup, 'transform',
      'translate(' + centerX + ' ' + (geometry.top + THRESHOLD_HANDLE_OFFSET_Y) + ')');
    setAttr(chartRefs.lowerGroup, 'transform',
      'translate(' + lowerX + ' ' + (geometry.top + SENSITIVITY_HANDLE_OFFSET_Y) + ')');
    setAttr(chartRefs.upperGroup, 'transform',
      'translate(' + upperX + ' ' + (geometry.top + SENSITIVITY_HANDLE_OFFSET_Y) + ')');
    chartRefs.thresholdGroup.setAttribute('aria-valuenow', String(state.threshold));
    const thresholdLabel = tr('aria_threshold', {
      lux: formatLux(state.threshold),
    });
    chartRefs.thresholdGroup.setAttribute('aria-label', thresholdLabel);
    chartRefs.thresholdGroup.setAttribute('aria-valuetext', thresholdLabel);
    chartRefs.lowerGroup.setAttribute('aria-valuenow', String(state.sensitivity));
    chartRefs.upperGroup.setAttribute('aria-valuenow', String(state.sensitivity));
    chartRefs.lowerGroup.setAttribute('aria-valuemin', '0');
    chartRefs.lowerGroup.setAttribute('aria-valuemax', String(state.threshold));
    chartRefs.lowerGroup.setAttribute('aria-valuenow', String(Math.max(0, state.threshold - state.sensitivity)));
    chartRefs.upperGroup.setAttribute('aria-valuemin', String(state.threshold));
    chartRefs.upperGroup.setAttribute('aria-valuemax', String(MAX_THRESHOLD_LUX));
    chartRefs.upperGroup.setAttribute('aria-valuenow', String(state.threshold + state.sensitivity));
    const lowerLabel = tr('aria_sensitivity_lower', {
      lux: formatLux(lowerLux),
      sensitivity: formatLux(state.sensitivity),
    });
    chartRefs.lowerGroup.setAttribute('aria-label', lowerLabel);
    chartRefs.lowerGroup.setAttribute('aria-valuetext', lowerLabel);
    const upperLabel = tr('aria_sensitivity_upper', {
      lux: formatLux(upperLux),
      sensitivity: formatLux(state.sensitivity),
    });
    chartRefs.upperGroup.setAttribute('aria-label', upperLabel);
    chartRefs.upperGroup.setAttribute('aria-valuetext', upperLabel);
  }

  function renderLive(domain, geometry) {
    const hasReading = Number.isFinite(state.liveLux) && state.liveLux >= 0;
    [chartRefs.liveLine, chartRefs.livePulse, chartRefs.livePoint,
      chartRefs.liveLabelGroup, chartRefs.liveOverflow].forEach(node => {
      node.style.display = hasReading ? '' : 'none';
    });
    if (!hasReading) return;

    const clampedLux = Math.max(domain.min, Math.min(domain.max, state.liveLux));
    const x = xForLux(clampedLux, domain, geometry);
    const y = yForPercent(pointLevelAt(state.liveLux), geometry);
    setAttr(chartRefs.liveLine, 'x1', x);
    setAttr(chartRefs.liveLine, 'x2', x);
    setAttr(chartRefs.liveLine, 'y1', geometry.top);
    setAttr(chartRefs.liveLine, 'y2', geometry.bottom);
    setAttr(chartRefs.livePulse, 'cx', x);
    setAttr(chartRefs.livePulse, 'cy', y);
    setAttr(chartRefs.livePoint, 'cx', x);
    setAttr(chartRefs.livePoint, 'cy', y);

    const outside = state.liveLux < domain.min || state.liveLux > domain.max;
    if (outside) {
      setAttr(chartRefs.liveOverflow, 'x', state.liveLux < domain.min ? geometry.left + 8 : geometry.right - 8);
      setAttr(chartRefs.liveOverflow, 'y', geometry.top + 15);
      setText(chartRefs.liveOverflow, state.liveLux < domain.min ? '‹' : '›');
    }
    chartRefs.liveOverflow.style.display = outside ? '' : 'none';

    const label = tr('now') + ' · ' + formatLux(state.liveLux) + ' lx · ' +
      Math.round(pointLevelAt(state.liveLux)) + '%';
    const width = Math.max(94, label.length * 5.8 + 14);
    const labelX = Math.max(geometry.left, Math.min(geometry.right - width, x - width / 2));
    const labelY = y <= geometry.top + 34
      ? Math.min(geometry.bottom - 20, y + 10)
      : Math.max(2, y - 28);
    setAttr(chartRefs.liveLabelGroup, 'transform', 'translate(' + labelX + ' ' + labelY + ')');
    setAttr(chartRefs.liveLabelBg, 'width', width);
    setAttr(chartRefs.liveLabel, 'x', width / 2);
    setText(chartRefs.liveLabel, label);
  }

  function renderRugs(domain, geometry) {
    for (const [samples, refs] of [
      [state.yesterday, chartRefs.rugYesterday],
      [state.today, chartRefs.rugToday],
    ]) {
      let index = 0;
      (samples || []).forEach(value => {
        const ref = refs[index++];
        if (!Number.isFinite(value) || value < 0) {
          ref.style.display = 'none';
          return;
        }
        const x = xForLux(Math.max(MIN_LUX, value), domain, geometry);
        ref.style.display = '';
        setAttr(ref, 'x1', x);
        setAttr(ref, 'x2', x);
        setAttr(ref, 'y1', geometry.bottom - 3);
        setAttr(ref, 'y2', geometry.bottom);
      });
      while (index < refs.length) refs[index++].style.display = 'none';
    }
  }

  function renderChart() {
    const domain = activeDomain();
    const geometry = plotGeometry();
    state.domain = domain;
    setAttr(chartRefs.background, 'x', geometry.left);
    setAttr(chartRefs.background, 'y', geometry.top);
    setAttr(chartRefs.background, 'width', geometry.width);
    setAttr(chartRefs.background, 'height', geometry.height);
    renderAxes(domain, geometry);
    renderCurve(domain, geometry);
    renderThreshold(domain, geometry);
    renderRugs(domain, geometry);
    renderLive(domain, geometry);
  }

  function historyY(lux, domain, top, bottom) {
    const safeLux = Math.max(domain.min, Math.min(domain.max, Math.max(MIN_LUX, lux)));
    const ratio = (Math.log10(safeLux) - Math.log10(domain.min)) /
      (Math.log10(domain.max) - Math.log10(domain.min));
    return bottom - Math.max(0, Math.min(1, ratio)) * (bottom - top);
  }

  function historyCurveSegments(samples, domain, left, plotWidth, top, bottom) {
    const segments = [];
    let segment = [];
    for (let hour = 0; hour < HISTORY_HOURS; hour++) {
      const value = samples && Number.isFinite(samples[hour]) ? samples[hour] : EMPTY_SAMPLE;
      if (value < 0) {
        if (segment.length) segments.push(segment);
        segment = [];
        continue;
      }
      segment.push({
        hour,
        value,
        x: left + (hour + 0.5) * plotWidth / HISTORY_HOURS,
        y: historyY(value, domain, top, bottom),
      });
    }
    if (segment.length) segments.push(segment);
    return segments;
  }

  function smoothHistorySegment(points) {
    if (!points.length) return '';
    let path = 'M' + points[0].x.toFixed(2) + ' ' + points[0].y.toFixed(2);
    for (let i = 1; i < points.length; i++) {
      const previous = points[i - 2] || points[i - 1];
      const start = points[i - 1];
      const end = points[i];
      const next = points[i + 1] || end;
      const minY = Math.min(start.y, end.y);
      const maxY = Math.max(start.y, end.y);
      const control1Y = Math.max(minY, Math.min(maxY,
        start.y + (end.y - previous.y) / 6));
      const control2Y = Math.max(minY, Math.min(maxY,
        end.y - (next.y - start.y) / 6));
      const control1X = start.x + (end.x - previous.x) / 6;
      const control2X = end.x - (next.x - start.x) / 6;
      path += ' C' + control1X.toFixed(2) + ' ' + control1Y.toFixed(2) +
        ' ' + control2X.toFixed(2) + ' ' + control2Y.toFixed(2) +
        ' ' + end.x.toFixed(2) + ' ' + end.y.toFixed(2);
    }
    return path;
  }

  function historyPaths(segments, baseline) {
    const paths = segments.filter(segment => segment.length > 1)
      .map(segment => smoothHistorySegment(segment));
    const areas = segments.filter(segment => segment.length > 1)
      .map(segment => {
        const line = smoothHistorySegment(segment);
        const first = segment[0];
        const last = segment[segment.length - 1];
        return line + ' L' + last.x.toFixed(2) + ' ' + baseline.toFixed(2) +
          ' L' + first.x.toFixed(2) + ' ' + baseline.toFixed(2) + ' Z';
      });
    return { line: paths.join(' '), area: areas.join(' ') };
  }

  function renderHistory() {
    const domain = activeDomain();
    const left = 42;
    const right = viewWidth - 14;
    const top = 28;
    const bottom = HISTORY_HEIGHT - 24;
    const plotWidth = right - left;
    const hourSlot = plotWidth / 24;
    const low = Math.max(domain.min, state.threshold - state.sensitivity);
    const high = Math.max(low, state.threshold + state.sensitivity);
    const thresholdY = historyY(Math.max(MIN_LUX, state.threshold), domain, top, bottom);
    const lowerY = historyY(Math.max(MIN_LUX, low), domain, top, bottom);
    const upperY = historyY(Math.max(MIN_LUX, high), domain, top, bottom);
    setAttr(chartRefs.historyBand, 'x', left);
    setAttr(chartRefs.historyBand, 'y', upperY);
    setAttr(chartRefs.historyBand, 'width', plotWidth);
    setAttr(chartRefs.historyBand, 'height', Math.max(0, lowerY - upperY));
    setAttr(chartRefs.historyThreshold, 'x1', left);
    setAttr(chartRefs.historyThreshold, 'x2', right);
    setAttr(chartRefs.historyThreshold, 'y1', thresholdY);
    setAttr(chartRefs.historyThreshold, 'y2', thresholdY);
    setAttr(chartRefs.historyBaseline, 'x1', left);
    setAttr(chartRefs.historyBaseline, 'x2', right);
    setAttr(chartRefs.historyBaseline, 'y1', bottom);
    setAttr(chartRefs.historyBaseline, 'y2', bottom);
    [1000, 10, MIN_LUX].forEach((lux, index) => {
      const label = chartRefs.historyLuxLabels[index];
      setAttr(label, 'x', left - 4);
      setAttr(label, 'y', historyY(lux, domain, top, bottom) + 3);
      setText(label, formatLux(lux) + ' lx');
    });

    const noData = !state.historyLoaded ||
      !(state.today || []).some(value => Number.isFinite(value) && value >= 0) &&
      !(state.yesterday || []).some(value => Number.isFinite(value) && value >= 0);
    const todaySegments = historyCurveSegments(state.today, domain, left, plotWidth, top, bottom);
    const yesterdaySegments = historyCurveSegments(state.yesterday, domain, left, plotWidth, top, bottom);
    const todayPaths = historyPaths(todaySegments, bottom);
    const yesterdayPaths = historyPaths(yesterdaySegments, bottom);
    chartRefs.historyBand.style.display = noData ? 'none' : '';
    chartRefs.historyThreshold.style.display = noData ? 'none' : '';
    setText(chartRefs.noReadings, state.historyLoaded ? tr('no_readings') : '');
    chartRefs.noReadings.style.display = noData && state.historyLoaded ? '' : 'none';
    setAttr(chartRefs.noReadings, 'x', viewWidth / 2);
    setAttr(chartRefs.noReadings, 'y', HISTORY_HEIGHT / 2 + 4);
    setAttr(chartRefs.historyTodayArea, 'd', todayPaths.area);
    setAttr(chartRefs.historyYesterdayCurve, 'd', yesterdayPaths.line);
    setAttr(chartRefs.historyTodayCurve, 'd', todayPaths.line);
    chartRefs.historyTodayArea.style.display = noData || !todayPaths.area ? 'none' : '';
    chartRefs.historyYesterdayCurve.style.display = noData || !yesterdayPaths.line ? 'none' : '';
    chartRefs.historyTodayCurve.style.display = noData || !todayPaths.line ? 'none' : '';

    const currentHour = state.hour >= 0 && state.hour < 24 ? state.hour : -1;
    for (let hour = 0; hour < 24; hour++) {
      const todayValue = state.today && Number.isFinite(state.today[hour]) ? state.today[hour] : EMPTY_SAMPLE;
      const yesterdayValue = state.yesterday && Number.isFinite(state.yesterday[hour])
        ? state.yesterday[hour] : EMPTY_SAMPLE;
      const center = left + (hour + 0.5) * hourSlot;
      for (const [ref, value, isToday] of [
        [historyRefs.yesterday[hour], yesterdayValue, false],
        [historyRefs.today[hour], todayValue, true],
      ]) {
        if (value < 0) {
          ref.style.display = 'none';
          continue;
        }
        const y = historyY(value, domain, top, bottom);
        ref.style.display = noData ? 'none' : '';
        setAttr(ref, 'cx', center);
        setAttr(ref, 'cy', y);
        setAttr(ref, 'r', isToday && hour === currentHour ? 4 : 2.5);
        ref.setAttribute('title', pad2(hour) + ':00 · ' + formatLux(value) + ' lx');
        ref.classList.toggle('is-current', isToday && hour === currentHour);
      }
      const empty = chartRefs.historyEmpty[hour];
      const bothEmpty = todayValue < 0 && yesterdayValue < 0;
      empty.style.display = bothEmpty && state.historyLoaded && !noData ? '' : 'none';
      setAttr(empty, 'cx', center);
      setAttr(empty, 'cy', bottom - 2);
    }

    chartRefs.historyCurrent.style.display =
      currentHour >= 0 && state.historyLoaded && !noData ? '' : 'none';
    if (currentHour >= 0) {
      setAttr(chartRefs.historyCurrent, 'x', left + currentHour * hourSlot + 1);
      setAttr(chartRefs.historyCurrent, 'y', top - 1);
      setAttr(chartRefs.historyCurrent, 'width', Math.max(1, hourSlot - 2));
      setAttr(chartRefs.historyCurrent, 'height', bottom - top + 2);
    }

    const ticks = [0, 6, 12, 18, 24];
    ticks.forEach((tick, i) => {
      const x = left + plotWidth * tick / 24;
      const line = chartRefs.historyTicks[i];
      const label = chartRefs.historyTickLabels[i];
      setAttr(line, 'x1', x);
      setAttr(line, 'x2', x);
      setAttr(line, 'y1', top);
      setAttr(line, 'y2', bottom);
      setAttr(label, 'x', x);
      setAttr(label, 'y', HISTORY_HEIGHT - 5);
      line.style.display = noData ? 'none' : '';
    });

    setText(chartRefs.historyLegendLabel, tr('history'));
    setText(chartRefs.yesterdayLegend, tr('yesterday'));
    setText(chartRefs.todayLegend, tr('today'));

  }

  function render() {
    if (!chartRefs.background) return;
    renderChart();
    renderHistory();
  }

  function updateSize() {
    const width = chart.getBoundingClientRect
      ? Math.round(chart.getBoundingClientRect().width)
      : WIDTH_FALLBACK;
    viewWidth = Math.max(280, width || WIDTH_FALLBACK);
    setAttr(chart, 'viewBox', '0 0 ' + viewWidth + ' ' + CHART_HEIGHT);
    setAttr(historyChart, 'viewBox', '0 0 ' + viewWidth + ' ' + HISTORY_HEIGHT);
    setAttr(chartRefs.historyLegendLabel, 'x', 5);
    setAttr(chartRefs.yesterdayLegendSwatch, 'x1', Math.max(115, viewWidth - 142));
    setAttr(chartRefs.yesterdayLegendSwatch, 'x2', Math.max(123, viewWidth - 134));
    setAttr(chartRefs.yesterdayLegend, 'x', Math.max(125, viewWidth - 132));
    setAttr(chartRefs.todayLegendSwatch, 'x1', Math.max(210, viewWidth - 76));
    setAttr(chartRefs.todayLegendSwatch, 'x2', Math.max(218, viewWidth - 68));
    setAttr(chartRefs.todayLegend, 'x', Math.max(220, viewWidth - 66));
    render();
  }

  function snapLux(value, precise = false) {
    const safeValue = Math.max(MIN_LUX, value);
    const step = precise
      ? 0.1
      : Math.max(0.1, Math.pow(10, Math.floor(Math.log10(safeValue)) - 1));
    return Math.round(safeValue / step) * step;
  }

  function minMaxLux(index) {
    const previous = index > 0 ? state.points[index - 1].lux * MIN_LOG_GAP_RATIO : MIN_LUX;
    const next = index < POINT_COUNT - 1
      ? state.points[index + 1].lux / MIN_LOG_GAP_RATIO : MAX_LUX;
    return {
      min: Math.max(MIN_LUX, Math.ceil(previous * 10 - 1e-7) / 10),
      max: Math.min(MAX_LUX, Math.floor(next * 10 + 1e-7) / 10),
    };
  }

  function clampPoint(index, lux, pct, precise = false) {
    const bounds = minMaxLux(index);
    if (bounds.min > bounds.max) return null;
    const rawLux = snapLux(lux, precise);
    const rawPct = Math.round(pct);
    return {
      lux: Math.max(bounds.min, Math.min(bounds.max, rawLux)),
      pct: Math.max(MIN_LEVEL, Math.min(MAX_LEVEL, rawPct)),
    };
  }

  function updateThresholdError(message) {
    const invalid = !!message;
    [chartRefs.thresholdGroup, chartRefs.lowerGroup, chartRefs.upperGroup].forEach(handle => {
      if (handle) handle.setAttribute('aria-invalid', invalid ? 'true' : 'false');
    });
    if (thresholdError) {
      thresholdError.hidden = !invalid;
      setText(thresholdError, message || '');
    }
  }

  function validateThreshold() {
    let message = '';
    if (!Number.isFinite(state.threshold) ||
        state.threshold < 0 || state.threshold > MAX_THRESHOLD_LUX) {
      message = tr('range_error', { min: 0, max: MAX_THRESHOLD_LUX });
    } else if (!Number.isFinite(state.sensitivity) ||
        state.sensitivity < 0 || state.sensitivity > MAX_SENSITIVITY_LUX) {
      message = tr('range_error', { min: 0, max: MAX_SENSITIVITY_LUX });
    } else if (state.sensitivity > state.threshold) {
      message = tr('threshold_error');
    }
    updateThresholdError(message);
    return !message;
  }

  /* Accepts only a consistent pair (sensitivity <= threshold); the caller
     falls back to resetThreshold() when the device sends anything else. */
  function setThreshold(threshold, sensitivity) {
    const t = number(threshold, 0, MAX_THRESHOLD_LUX);
    const s = number(sensitivity, 0, MAX_SENSITIVITY_LUX);
    if (t === null || s === null || s > t) return false;
    state.threshold = roundTenth(t);
    state.sensitivity = roundTenth(s);
    validateThreshold();
    render();
    return true;
  }

  function resetThreshold() {
    return setThreshold(DEFAULT_THRESHOLD, DEFAULT_SENSITIVITY);
  }

  function invalidPointIndex() {
    if (!Array.isArray(state.points) || state.points.length !== POINT_COUNT)
      return 0;
    for (let i = 0; i < POINT_COUNT; i++) {
      const point = state.points[i];
      if (!Number.isFinite(point.lux) || point.lux < MIN_LUX || point.lux > MAX_LUX ||
          !Number.isFinite(point.pct) || !Number.isInteger(point.pct) ||
          point.pct < MIN_LEVEL || point.pct > MAX_LEVEL ||
          (i > 0 && (point.lux <= state.points[i - 1].lux ||
            Math.log10(point.lux / state.points[i - 1].lux) < MIN_LOG_GAP - 1e-6)))
        return i;
    }
    return -1;
  }

  function isValid() {
    return invalidPointIndex() < 0 && validateThreshold();
  }

  function focusInvalid() {
    const invalidPoint = invalidPointIndex();
    if (invalidPoint >= 0) {
      pointRefs[invalidPoint].group.focus();
      return;
    }
    if (!validateThreshold()) {
      chartRefs.upperGroup.focus();
    }
  }

  function set(values) {
    if (!Array.isArray(values) || values.length !== POINT_COUNT * 2)
      return false;
    const points = [];
    for (let i = 0; i < POINT_COUNT; i++) {
      const lux = Number(values[i * 2]);
      const pct = Number(values[i * 2 + 1]);
      if (!Number.isFinite(lux) || !Number.isFinite(pct) ||
          lux < MIN_LUX || lux > MAX_LUX ||
          Math.abs(lux * 10 - Math.round(lux * 10)) > 0.001 ||
          !Number.isInteger(pct) || pct < MIN_LEVEL || pct > MAX_LEVEL ||
          (i > 0 && (lux <= points[i - 1].lux ||
            Math.log10(lux / points[i - 1].lux) < MIN_LOG_GAP - 1e-6)))
        return false;
      points.push({ lux, pct });
    }
    const animate = state.ready;
    state.points = points;
    state.ready = true;
    wrapper.dataset.state = 'ready';
    if (animate)
      animateCurveTo(points);
    else {
      state.displayPoints = points.map(point => ({ ...point }));
      render();
    }
    return true;
  }

  function resetCurve() {
    return set(DEFAULT_POINTS.flatMap(point => [point.lux, point.pct]));
  }

  function get() {
    const result = [];
    state.points.forEach(point => {
      result.push(Math.round(point.lux * 10) / 10, Math.round(point.pct));
    });
    return result;
  }

  function setHistory(data) {
    if (!data || !Array.isArray(data.t) || !Array.isArray(data.y) ||
        data.t.length !== HISTORY_HOURS || data.y.length !== HISTORY_HOURS)
      return false;
    state.today = data.t.map(value => value != null && Number.isFinite(Number(value)) && Number(value) >= 0
      ? Number(value) / 10 : EMPTY_SAMPLE);
    state.yesterday = data.y.map(value => value != null && Number.isFinite(Number(value)) && Number(value) >= 0
      ? Number(value) / 10 : EMPTY_SAMPLE);
    state.hour = data.h != null && Number.isInteger(Number(data.h)) ? Number(data.h) : -1;
    state.historyLoaded = true;
    render();
    return true;
  }

  function setLive(value) {
    const lux = value == null ? NaN : Number(value);
    state.liveLux = Number.isFinite(lux) && lux >= 0 ? lux : null;
    render();
  }

  function showStatus(message) {
    if (!statusNode) return;
    statusNode.hidden = !message;
    setText(statusNode, message || '');
  }

  function updateStatusMessage() {
    if (state.historyError)
      showStatus(tr('history_error'));
    else if (state.liveError)
      showStatus(tr('live_error'));
    else
      showStatus('');
  }

  function chartPoint(event) {
    const rect = chart.getBoundingClientRect();
    if (!rect.width || !rect.height) return null;
    return {
      x: (event.clientX - rect.left) * viewWidth / rect.width,
      y: (event.clientY - rect.top) * CHART_HEIGHT / rect.height,
    };
  }

  function distance(a, b) {
    return Math.hypot(a.x - b.x, a.y - b.y);
  }

  function hitTest(at, domain, geometry) {
    const lowLux = Math.max(MIN_LUX, state.threshold - state.sensitivity);
    const highLux = Math.max(MIN_LUX, state.threshold + state.sensitivity);
    const centerLux = Math.max(MIN_LUX, state.threshold);
    const visibleHandles = [
      {
        kind: 'threshold',
        center: { x: xForLux(centerLux, domain, geometry), y: geometry.top + THRESHOLD_HANDLE_OFFSET_Y },
      },
      {
        kind: 'lower',
        center: { x: xForLux(lowLux, domain, geometry), y: geometry.top + SENSITIVITY_HANDLE_OFFSET_Y },
      },
      {
        kind: 'upper',
        center: { x: xForLux(highLux, domain, geometry), y: geometry.top + SENSITIVITY_HANDLE_OFFSET_Y },
      },
    ];
    const visibleHandle = visibleHandles.find(handle => distance(at, handle.center) <= 10);
    if (visibleHandle) return visibleHandle;

    for (let i = 0; i < POINT_COUNT; i++) {
      const point = state.displayPoints[i];
      const center = { x: xForLux(point.lux, domain, geometry), y: yForPercent(point.pct, geometry) };
      if (distance(at, center) <= 18)
        return { kind: 'point', index: i, center };
    }

    const center = xForLux(centerLux, domain, geometry);
    const low = xForLux(lowLux, domain, geometry);
    const high = xForLux(highLux, domain, geometry);
    const handleTargets = visibleHandles
      .filter(handle => Math.abs(at.x - handle.center.x) <= HANDLE_HIT_RADIUS &&
        at.y >= geometry.top && at.y <= geometry.bottom)
      .sort((a, b) => {
        const score = handle => Math.hypot(
          at.x - handle.center.x,
          (at.y - handle.center.y) * 0.25
        );
        return score(a) - score(b);
      });
    if (handleTargets.length > 0)
      return handleTargets[0];
    if (Math.abs(at.x - center) <= 14 && at.y >= geometry.top - 12 && at.y <= geometry.bottom)
      return { kind: 'threshold', center: { x: center, y: geometry.top + 1 } };
    if (at.x >= low && at.x <= high && at.y >= geometry.top && at.y <= geometry.bottom)
      return { kind: 'threshold', center: { x: center, y: geometry.top + 1 } };
    return null;
  }

  function updatePointFromDrag(drag, at) {
    const geometry = plotGeometry();
    const rawLux = luxForX(at.x, drag.domain, geometry);
    const rawPct = ((geometry.bottom - at.y) / geometry.height) * 100;
    const adjustedLux = Math.pow(10, Math.log10(Math.max(MIN_LUX, rawLux)) + drag.grabLog);
    const adjustedPct = rawPct + drag.grabPct;
    const point = clampPoint(drag.index, adjustedLux, adjustedPct, drag.unsnapped);
    if (!point) return false;
    const old = state.points[drag.index];
    if (point.lux === old.lux && point.pct === old.pct) return false;
    state.points[drag.index] = point;
    state.displayPoints = state.points.map(value => ({ ...value }));
    return true;
  }

  function updateThresholdFromDrag(drag, at) {
    const geometry = plotGeometry();
    const lux = snapLux(luxForX(at.x, drag.domain, geometry) *
      Math.pow(10, drag.grabLog), true);
    const previous = { threshold: state.threshold, sensitivity: state.sensitivity };
    if (drag.kind === 'threshold') {
      state.threshold = roundTenth(Math.max(state.sensitivity, Math.min(MAX_THRESHOLD_LUX, lux)));
    } else if (drag.kind === 'lower') {
      const lower = Math.max(0, Math.min(state.threshold, lux));
      state.sensitivity = roundTenth(Math.min(MAX_SENSITIVITY_LUX, state.threshold - lower));
    } else if (drag.kind === 'upper') {
      const upper = Math.max(state.threshold, Math.min(MAX_THRESHOLD_LUX, lux));
      state.sensitivity = roundTenth(Math.min(MAX_SENSITIVITY_LUX, state.threshold,
        upper - state.threshold));
    }
    validateThreshold();
    return state.threshold !== previous.threshold || state.sensitivity !== previous.sensitivity;
  }

  function emitChange() {
    changeListeners.forEach(listener => listener());
  }

  function pointerDown(event) {
    if (state.animating) return;
    if (event.button !== undefined && event.button !== 0) return;
    const at = chartPoint(event);
    if (!at) return;
    const domain = computeDomain();
    const hit = hitTest(at, domain, plotGeometry());
    if (!hit) return;
    event.preventDefault();
    const drag = {
      kind: hit.kind,
      index: hit.index,
      domain,
      start: at,
      travel: 0,
      changed: false,
      unsnapped: !!event.shiftKey,
      grabLog: 0,
      grabPct: 0,
    };
    if (hit.kind === 'point') {
      drag.grabLog = Math.log10(state.displayPoints[hit.index].lux) -
        Math.log10(luxForX(at.x, domain, plotGeometry()));
      drag.grabPct = state.displayPoints[hit.index].pct -
        ((plotGeometry().bottom - at.y) / plotGeometry().height) * 100;
    } else {
      const handleLux = hit.kind === 'threshold'
        ? Math.max(MIN_LUX, state.threshold)
        : hit.kind === 'lower'
          ? Math.max(MIN_LUX, state.threshold - state.sensitivity)
          : Math.max(MIN_LUX, state.threshold + state.sensitivity);
      drag.grabLog = Math.log10(handleLux) -
        Math.log10(luxForX(at.x, domain, plotGeometry()));
    }
    state.drag = drag;
    wrapper.classList.add('is-dragging');
    if (typeof chart.setPointerCapture === 'function')
      chart.setPointerCapture(event.pointerId);
    render();
  }

  function pointerMove(event) {
    const at = chartPoint(event);
    if (!at) return;
    if (!state.drag) {
      const hit = hitTest(at, computeDomain(), plotGeometry());
      const hover = hit && hit.kind === 'point' ? hit.index : -1;
      if (hover !== state.hoverPoint) {
        state.hoverPoint = hover;
        render();
      }
      chart.style.cursor = hit ? 'grab' : '';
      return;
    }

    const drag = state.drag;
    drag.travel = Math.max(drag.travel, distance(at, drag.start));
    if (drag.travel < TOUCH_SLOP) return;
    drag.unsnapped = !!event.shiftKey;
    if (drag.kind === 'point')
      drag.changed = updatePointFromDrag(drag, at) || drag.changed;
    else
      drag.changed = updateThresholdFromDrag(drag, at) || drag.changed;
    render();
  }

  function focusRelatedControl(drag) {
    if (drag.kind === 'point') {
      pointRefs[drag.index].group.focus();
      return;
    }
    const handle = drag.kind === 'threshold'
      ? chartRefs.thresholdGroup
      : drag.kind === 'lower' ? chartRefs.lowerGroup : chartRefs.upperGroup;
    handle.focus();
  }

  function pointerEnd() {
    const drag = state.drag;
    if (!drag) return;
    state.drag = null;
    wrapper.classList.remove('is-dragging');
    chart.style.cursor = '';
    if (drag.travel < TOUCH_SLOP)
      focusRelatedControl(drag);
    else if (drag.changed) {
      emitChange();
      updateStatusMessage();
    }
    render();
  }

  function movePointByKeyboard(index, event) {
    if (state.animating) return false;
    const point = state.points[index];
    let lux = point.lux;
    let pct = point.pct;
    if (event.key === 'ArrowLeft')
      lux /= Math.pow(10, 0.05);
    else if (event.key === 'ArrowRight')
      lux *= Math.pow(10, 0.05);
    else if (event.key === 'ArrowUp')
      pct += event.shiftKey ? 10 : 1;
    else if (event.key === 'ArrowDown')
      pct -= event.shiftKey ? 10 : 1;
    else if (event.key === 'Home') {
      const bounds = minMaxLux(index);
      lux = bounds.min;
      pct = MIN_LEVEL;
    } else if (event.key === 'End') {
      const bounds = minMaxLux(index);
      lux = bounds.max;
      pct = MAX_LEVEL;
    } else return false;

    const next = clampPoint(index, lux, pct, event.shiftKey);
    if (!next) return false;
    state.points[index] = next;
    state.displayPoints = state.points.map(value => ({ ...value }));
    render();
    emitChange();
    return true;
  }

  function moveThresholdByKeyboard(kind, event) {
    const previous = { threshold: state.threshold, sensitivity: state.sensitivity };
    if (event.key === 'ArrowLeft' || event.key === 'ArrowRight') {
      const direction = event.key === 'ArrowLeft' ? -1 : 1;
      let next = roundTenth(snapLux(state.threshold * Math.pow(10, direction * 0.05), true));
      // Near 0.1 lux the logarithmic step rounds back to the same value.
      if (next === state.threshold)
        next = roundTenth(state.threshold + direction * LUX_STEP);
      state.threshold = roundTenth(Math.max(state.sensitivity, Math.min(MAX_THRESHOLD_LUX, next)));
    } else if (kind !== 'threshold' && (event.key === 'ArrowUp' || event.key === 'ArrowDown')) {
      const direction = event.key === 'ArrowUp' ? 1 : -1;
      const step = event.shiftKey ? COARSE_LUX_STEP : LUX_STEP;
      state.sensitivity = roundTenth(Math.max(0, Math.min(MAX_SENSITIVITY_LUX, state.threshold,
        state.sensitivity + direction * step)));
    } else {
      return false;
    }
    validateThreshold();
    render();
    if (state.threshold !== previous.threshold || state.sensitivity !== previous.sensitivity)
      emitChange();
    return true;
  }

  function handlePointKey(index, event) {
    if (movePointByKeyboard(index, event)) event.preventDefault();
  }

  function handleThresholdKey(kind, event) {
    if (moveThresholdByKeyboard(kind, event)) event.preventDefault();
  }

  function bindChartInteractions() {
    for (let i = 0; i < POINT_COUNT; i++) {
      pointRefs[i].group.addEventListener('keydown', event => handlePointKey(i, event));
    }

    chartRefs.thresholdGroup.addEventListener('keydown', event => handleThresholdKey('threshold', event));
    chartRefs.lowerGroup.addEventListener('keydown', event => handleThresholdKey('lower', event));
    chartRefs.upperGroup.addEventListener('keydown', event => handleThresholdKey('upper', event));

    chart.addEventListener('pointerdown', pointerDown);
    chart.addEventListener('pointermove', pointerMove);
    chart.addEventListener('pointerup', pointerEnd);
    chart.addEventListener('pointercancel', pointerEnd);
    chart.addEventListener('pointerleave', () => {
      if (!state.drag && state.hoverPoint !== -1) {
        state.hoverPoint = -1;
        render();
      }
    });

    if (typeof ResizeObserver === 'function') {
      resizeObserver = new ResizeObserver(updateSize);
      resizeObserver.observe(chart);
    } else {
      window.addEventListener('resize', updateSize, { passive: true });
    }
  }

  function canPoll() {
    const page = el('tab-settings');
    const panel = wrapper.closest('.panel.collapsible');
    return !!page && page.classList.contains('active') &&
      !dimBright.hidden && dimMode.value === '0' && !document.hidden &&
      (!panel || !panel.classList.contains('collapsed'));
  }

  async function fetchHistory() {
    if (!state.pollTimer || !canPoll()) return;
    if (state.requestBusy) {
      state.pendingHistory = true;
      return;
    }
    state.requestBusy = true;
    state.pendingHistory = false;
    try {
      const result = await apiGet('/api/status?luxhist=1');
      if (!result.ok || !result.data || !setHistory(result.data.luxhist))
        throw new Error('Invalid ambient light history response');
      state.historyError = false;
      state.hour = result.data.local_hour != null && Number.isInteger(Number(result.data.local_hour))
        ? Number(result.data.local_hour) : state.hour;
      updateStatusMessage();
      render();
    } catch (error) {
      state.historyError = true;
      console.warn('Ambient light history request failed', error);
      updateStatusMessage();
    } finally {
      state.requestBusy = false;
      if (state.pendingHistory && state.pollTimer && canPoll())
        fetchHistory();
    }
  }

  async function pollLive() {
    if (!state.pollTimer || !canPoll() || state.requestBusy) return;
    state.requestBusy = true;
    try {
      const result = await apiGet('/api/status?light=1');
      if (!result.ok || !result.data)
        throw new Error('Status request failed');
      if (result.data.lux != null && Number.isFinite(Number(result.data.lux)))
        setLive(Number(result.data.lux));
      else
        setLive(null);
      const hour = result.data.local_hour == null ? -1 : Number(result.data.local_hour);
      if (Number.isInteger(hour) && hour >= 0 && hour < 24 && hour !== state.hour) {
        state.hour = hour;
        state.pendingHistory = true;
      }
      state.liveError = false;
      updateStatusMessage();
    } catch (error) {
      setLive(null);
      state.liveError = true;
      console.warn('Live ambient light request failed', error);
      updateStatusMessage();
    } finally {
      state.requestBusy = false;
      if (state.pendingHistory && state.pollTimer && canPoll())
        fetchHistory();
    }
  }

  function syncPolling() {
    const active = canPoll();
    if (active && state.pollTimer === null) {
      state.pollTimer = setInterval(pollLive, POLL_INTERVAL_MS);
      fetchHistory();
      pollLive();
    } else if (!active && state.pollTimer !== null) {
      clearInterval(state.pollTimer);
      state.pollTimer = null;
      state.pendingHistory = false;
    }
  }

  function refreshI18n() {
    chart.setAttribute('aria-label', tr('title'));
    historyChart.setAttribute('aria-label', tr('history'));
    validateThreshold();
    render();
  }

  function initPollingObservers() {
    const navLinks = $$('#nav a');
    navLinks.forEach(link => link.addEventListener('click', () => {
      window.setTimeout(syncPolling, 0);
    }));
    document.addEventListener('visibilitychange', syncPolling);
    dimMode.addEventListener('change', syncPolling);

    if (typeof MutationObserver === 'function') {
      activePanelObserver = new MutationObserver(syncPolling);
      const page = el('tab-settings');
      const panel = wrapper.closest('.panel.collapsible');
      [page, panel, dimBright].forEach(node => {
        if (node) activePanelObserver.observe(node, { attributes: true, attributeFilter: ['class', 'hidden'] });
      });
    }
  }

  const api = {
    set,
    get,
    setThreshold,
    resetThreshold,
    resetCurve,
    getThreshold: () => ({
      threshold: roundTenth(state.threshold),
      sensitivity: roundTenth(state.sensitivity),
    }),
    onChange(listener) {
      if (typeof listener === 'function') changeListeners.push(listener);
    },
    open() {
      syncPolling();
    },
    close() {
      if (state.pollTimer !== null) clearInterval(state.pollTimer);
      state.pollTimer = null;
      state.pendingHistory = false;
    },
    sync: syncPolling,
    refreshI18n,
    setLive,
    setHistory,
    isValid,
    focusInvalid,
  };

  window.BrightnessCurve = api;
  api.onChange(() => {
    if (window.saveBar) window.saveBar.markDirty();
  });

  createChartScene();
  bindChartInteractions();
  initPollingObservers();
  refreshI18n();
  render();
})();
