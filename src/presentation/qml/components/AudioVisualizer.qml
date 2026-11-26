import QtQuick
import QtQuick.Layouts
import ".."

// Cartão de áudio do Dashboard: medidores RMS/pico, forma de onda e espectro (FFT).
Rectangle {
    Layout.fillWidth: true; height: 140; color: Theme.cardColor; radius: 8; border.color: Theme.borderColor
    clip: true
    RowLayout {
        anchors.fill: parent; anchors.margins: 15; spacing: 15

        ColumnLayout {
            Layout.preferredWidth: 80
            Text { text: qsTr("AUDIO"); color: Theme.highlightColor; font.bold: true; font.pixelSize: 10 }
            RowLayout {
                spacing: 4
                Rectangle { width: 6; height: 50; color: Theme.successColor; opacity: 0.2; radius: 2
                    Rectangle { anchors.bottom: parent.bottom; width: 6; height: parent.height * Math.min(1.0, audioViewModel.rms * 2); color: Theme.successColor; radius: 2 }
                }
                Rectangle { width: 6; height: 50; color: Theme.criticalColor; opacity: 0.2; radius: 2
                    Rectangle { anchors.bottom: parent.bottom; width: 6; height: parent.height * Math.min(1.0, audioViewModel.peak); color: Theme.criticalColor; radius: 2 }
                }
            }
            Text { text: (audioViewModel.rms * 100).toFixed(0) + "%"; color: Theme.textColor; font.pixelSize: 9; Layout.alignment: Qt.AlignHCenter }
        }

        // Forma de onda
        Canvas {
            id: audioCanvas
            Layout.fillWidth: true; Layout.fillHeight: true
            onPaint: {
                var ctx = getContext("2d");
                ctx.clearRect(0, 0, width, height);
                ctx.strokeStyle = Theme.highlightColor;
                ctx.globalAlpha = 0.8;
                ctx.lineWidth = 2.0;
                ctx.beginPath();

                var data = audioViewModel.waveform;
                if (!data || data.length === 0) return;

                var sliceWidth = width / data.length;
                var x = 0;
                for (var i = 0; i < data.length; i++) {
                    // Ganho visual vem do ViewModel (MC-05).
                    var v = data[i] * audioViewModel.waveformGain;
                    if (v > 1.0) v = 1.0; else if (v < -1.0) v = -1.0;

                    var y = (v + 1) / 2 * height;
                    if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
                    x += sliceWidth;
                }
                ctx.stroke();
            }

            Connections {
                target: audioViewModel
                function onAudioDataChanged() {
                    audioCanvas.requestPaint();
                    spectrumCanvas.requestPaint();
                }
            }
        }

        // Espectro (FFT)
        Canvas {
            id: spectrumCanvas
            Layout.fillWidth: true; Layout.fillHeight: true
            onPaint: {
                var ctx = getContext("2d");
                ctx.clearRect(0, 0, width, height);
                var data = audioViewModel.spectrum;
                if (!data || data.length === 0) return;

                var barWidth = width / data.length;
                ctx.fillStyle = Theme.highlightColor;

                for (var i = 0; i < data.length; i++) {
                    // Ganho do espectro vem do ViewModel; sqrt para compressão dinâmica.
                    var magnitude = Math.sqrt(data[i]) * audioViewModel.spectrumGain;
                    var h = Math.min(height, magnitude * height);
                    ctx.fillRect(i * barWidth, height - h, barWidth - 1, h);
                }
            }
        }
    }
}
