pragma Singleton
import QtQuick

// Helpers de formatação compartilhados pelas views (registrado em ./qmldir).
QtObject {
    // Formatação humana de bytes (KiB/MiB/GiB/TiB).
    function formatBytes(n) {
        if (n === undefined || n === null || n <= 0) return "—";
        var units = ["B", "KiB", "MiB", "GiB", "TiB", "PiB"];
        var i = 0;
        var v = n;
        while (v >= 1024 && i < units.length - 1) { v /= 1024; i++; }
        return v.toFixed(i === 0 ? 0 : 1) + " " + units[i];
    }
}
