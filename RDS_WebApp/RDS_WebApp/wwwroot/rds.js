window.getSvgSize = function (elementId) {
    const el = document.getElementById(elementId);
    if (!el) return { width: 800, height: 500 };
    return { width: el.clientWidth, height: el.clientHeight };
};
