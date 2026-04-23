pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import AutoClicker as AC

Rectangle {
    id: root
    readonly property int contentMargin: 12
    implicitHeight: contentLayout.implicitHeight + (root.contentMargin * 2)
    height: root.implicitHeight
    color: "#2d2d2d"
    radius: 10
    border.color: root.isActive ? "#4CAF50" : "#444444"
    border.width: root.isActive ? 3 : 1

    // Simply making these 'required' allows ListView to map them automatically!
    required property int profileId
    required property string targetButton
    required property int targetButtonCode
    required property string keybind
    required property int keybindCode
    required property double frequency
    required property bool randomFrequencyEnabled
    required property double maxFrequency
    required property int mode
    required property bool isActive
    readonly property int controlHeight: 36
    readonly property int labelHeight: 16
    readonly property int secondaryRowGap: root.randomFrequencyEnabled ? 18 : 16

    function normalizedFrequencyText(value) {
        return Number(value).toFixed(2);
    }

    function commitFrequency() {
        const parsed = Number.parseFloat(frequencyField.text.replace(",", "."));
        if (Number.isNaN(parsed) || parsed <= 0) {
            frequencyField.text = root.normalizedFrequencyText(root.frequency);
            return;
        }
        const clamped = Math.max(0.01, Math.min(100.0, parsed));
        frequencyField.text = root.normalizedFrequencyText(clamped);
        AC.AutoClickerController.setFrequency(root.profileId, clamped);
    }

    function commitMaxFrequency() {
        const parsed = Number.parseFloat(maxFrequencyField.text.replace(",", "."));
        if (Number.isNaN(parsed) || parsed <= 0) {
            maxFrequencyField.text = root.normalizedFrequencyText(root.maxFrequency);
            return;
        }
        const clamped = Math.max(root.frequency, Math.min(100.0, parsed));
        maxFrequencyField.text = root.normalizedFrequencyText(clamped);
        AC.AutoClickerController.setMaxFrequency(root.profileId, clamped);
    }

    MouseArea {
        id: rootMA
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        radius: 10
        border.color: "#5a9fd4"
        border.width: rootMA.containsMouse ? 2 : 0
        Behavior on border.width {
            NumberAnimation {
                duration: 150
            }
        }
    }

    RowLayout {
        id: contentLayout
        anchors.fill: parent
        anchors.margins: root.contentMargin
        spacing: 12

        // Status Bar
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 6
            color: root.isActive ? "#4CAF50" : "#555555"
            radius: 3
        }

        // Settings Columns
        RowLayout {
            Layout.fillWidth: true
            spacing: 16

            // Column 1: Target & Hotkey
            ColumnLayout {
                Layout.fillHeight: true
                Layout.preferredWidth: 140
                spacing: 2

                Text {
                    Layout.preferredHeight: root.labelHeight
                    text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("target_button")
                    font.pixelSize: 11
                    color: "#888888"
                    verticalAlignment: Text.AlignVCenter
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.controlHeight
                    color: targetArea.containsMouse || (AC.AutoClickerController.isListeningForTarget && AC.AutoClickerController.listeningProfileId === root.profileId) ? "#3d5a80" : "#3a3a3a"
                    radius: 6
                    border.color: root.targetButton !== "" ? "#5a9fd4" : "#555555"
                    border.width: 2
                    Text {
                        anchors.centerIn: parent
                        text: root.targetButton !== "" ? root.targetButton : (AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("set_target"))
                        font.pixelSize: 13
                        color: root.targetButton !== "" ? "#ffffff" : "#777777"
                        font.bold: root.targetButton !== ""
                    }
                    MouseArea {
                        id: targetArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: AC.AutoClickerController.startListeningForTarget(root.profileId)
                    }
                }

                Item {
                    Layout.preferredHeight: root.secondaryRowGap
                }

                Text {
                    Layout.preferredHeight: root.labelHeight
                    text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("keybind")
                    font.pixelSize: 11
                    color: "#888888"
                    verticalAlignment: Text.AlignVCenter
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.controlHeight
                    color: keybindArea.containsMouse || (AC.AutoClickerController.isListeningForKeybind && AC.AutoClickerController.listeningProfileId === root.profileId) ? "#3d5a80" : "#3a3a3a"
                    radius: 6
                    border.color: root.keybind !== "" ? "#5a9fd4" : "#555555"
                    border.width: 2
                    Text {
                        anchors.centerIn: parent
                        text: root.keybind !== "" ? root.keybind : (AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("set_keybind"))
                        font.pixelSize: 13
                        color: root.keybind !== "" ? "#ffffff" : "#777777"
                        font.bold: root.keybind !== ""
                    }
                    MouseArea {
                        id: keybindArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: AC.AutoClickerController.startListeningForKeybind(root.profileId)
                    }
                }
            }

            // Column 2: Intervals
            ColumnLayout {
                Layout.fillHeight: true
                Layout.preferredWidth: 150
                spacing: 2

                Text {
                    Layout.preferredHeight: root.labelHeight
                    text: root.randomFrequencyEnabled ? (AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("min_frequency_sec")) : (AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("frequency_sec"))
                    font.pixelSize: 11
                    color: "#888888"
                    verticalAlignment: Text.AlignVCenter
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.controlHeight
                    color: "#3a3a3a"
                    radius: 6
                    border.color: frequencyField.activeFocus ? "#5a9fd4" : "#555555"
                    border.width: 1
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 2
                        spacing: 2
                        TextField {
                            id: frequencyField
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            text: root.normalizedFrequencyText(root.frequency)
                            validator: RegularExpressionValidator {
                                regularExpression: /^\d{0,3}[.,]?\d{0,2}$/
                            }
                            color: "#ffffff"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            selectByMouse: true
                            background: Item {}
                            onEditingFinished: {
                                root.commitFrequency();
                                frequencyField.focus = false;
                            }
                        }
                        ColumnLayout {
                            Layout.preferredWidth: 24
                            Layout.fillHeight: true
                            spacing: 1
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                color: incArea.pressed ? "#4CAF50" : (incArea.containsMouse ? "#45a049" : "#454545")
                                radius: 3
                                Text {
                                    anchors.centerIn: parent
                                    text: "+"
                                    font.pixelSize: 11
                                    color: "#ffffff"
                                }
                                MouseArea {
                                    id: incArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        frequencyField.text = root.normalizedFrequencyText(Math.min(100.0, root.frequency + 0.05));
                                        root.commitFrequency();
                                    }
                                }
                            }
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                color: decArea.pressed ? "#cc4444" : (decArea.containsMouse ? "#aa3333" : "#454545")
                                radius: 3
                                Text {
                                    anchors.centerIn: parent
                                    text: "-"
                                    font.pixelSize: 11
                                    color: "#ffffff"
                                }
                                MouseArea {
                                    id: decArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        frequencyField.text = root.normalizedFrequencyText(Math.max(0.01, root.frequency - 0.05));
                                        root.commitFrequency();
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: Math.max(root.secondaryRowGap, randomRow.implicitHeight)

                    RowLayout {
                        id: randomRow
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 6

                        Rectangle {
                            Layout.preferredWidth: 14
                            Layout.preferredHeight: 14
                            radius: 3
                            color: root.randomFrequencyEnabled ? "#4CAF50" : "#2f2f2f"
                            border.color: root.randomFrequencyEnabled ? "#4CAF50" : "#666666"

                            Text {
                                anchors.centerIn: parent
                                text: root.randomFrequencyEnabled ? "v" : ""
                                font.pixelSize: 9
                                color: "#ffffff"
                            }
                        }

                        Text {
                            text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("random_frequency")
                            font.pixelSize: 11
                            color: "#bbbbbb"
                            Layout.fillWidth: true
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: AC.AutoClickerController.setRandomFrequencyEnabled(root.profileId, !root.randomFrequencyEnabled)
                    }
                }

                ColumnLayout {
                    visible: root.randomFrequencyEnabled
                    Layout.fillWidth: true
                    spacing: 2
                    Text {
                        Layout.preferredHeight: root.labelHeight
                        text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("max_frequency_sec")
                        font.pixelSize: 11
                        color: "#888888"
                        verticalAlignment: Text.AlignVCenter
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.controlHeight
                        color: "#3a3a3a"
                        radius: 6
                        border.color: maxFrequencyField.activeFocus ? "#5a9fd4" : "#555555"
                        border.width: 1
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 2
                            spacing: 2
                            TextField {
                                id: maxFrequencyField
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: root.normalizedFrequencyText(root.maxFrequency)
                                validator: RegularExpressionValidator {
                                    regularExpression: /^\d{0,3}[.,]?\d{0,2}$/
                                }
                                color: "#ffffff"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                selectByMouse: true
                                background: Item {}
                                onEditingFinished: {
                                    root.commitMaxFrequency();
                                    maxFrequencyField.focus = false;
                                }
                            }
                            ColumnLayout {
                                Layout.preferredWidth: 24
                                Layout.fillHeight: true
                                spacing: 1
                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    color: mIncArea.pressed ? "#4CAF50" : (mIncArea.containsMouse ? "#45a049" : "#454545")
                                    radius: 3
                                    Text {
                                        anchors.centerIn: parent
                                        text: "+"
                                        font.pixelSize: 11
                                        color: "#ffffff"
                                    }
                                    MouseArea {
                                        id: mIncArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            maxFrequencyField.text = root.normalizedFrequencyText(Math.min(100.0, root.maxFrequency + 0.05));
                                            root.commitMaxFrequency();
                                        }
                                    }
                                }
                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    color: mDecArea.pressed ? "#cc4444" : (mDecArea.containsMouse ? "#aa3333" : "#454545")
                                    radius: 3
                                    Text {
                                        anchors.centerIn: parent
                                        text: "-"
                                        font.pixelSize: 11
                                        color: "#ffffff"
                                    }
                                    MouseArea {
                                        id: mDecArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            maxFrequencyField.text = root.normalizedFrequencyText(Math.max(root.frequency, root.maxFrequency - 0.05));
                                            root.commitMaxFrequency();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Column 3: Mode & Control
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 2

                Text {
                    text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("mode")
                    font.pixelSize: 11
                    color: "#888888"
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    spacing: 4
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: root.mode === 0 ? "#4CAF50" : (tglModeArea.containsMouse ? "#3a3a3a" : "#2a2a2a")
                        radius: 6

                        Text {
                            anchors.centerIn: parent
                            text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("toggle")
                            font.pixelSize: 11
                            color: root.mode === 0 ? "#ffffff" : "#aaaaaa"
                        }

                        MouseArea {
                            id: tglModeArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: AC.AutoClickerController.setMode(root.profileId, 0)
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: root.mode === 1 ? "#4CAF50" : (hldModeArea.containsMouse ? "#3a3a3a" : "#2a2a2a")
                        radius: 6

                        Text {
                            anchors.centerIn: parent
                            text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("hold")
                            font.pixelSize: 11
                            color: root.mode === 1 ? "#ffffff" : "#aaaaaa"
                        }

                        MouseArea {
                            id: hldModeArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: AC.AutoClickerController.setMode(root.profileId, 1)
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                // Action Row
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    spacing: 6

                    // Status Text
                    Item {
                        Layout.preferredWidth: 72
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.centerIn: parent
                            width: parent.width
                            spacing: 1

                            Rectangle {
                                Layout.preferredWidth: 8
                                Layout.preferredHeight: 8
                                radius: 4
                                color: root.isActive ? "#4CAF50" : "#888888"
                                Layout.alignment: Qt.AlignHCenter

                                SequentialAnimation on color {
                                    running: root.isActive
                                    loops: Animation.Infinite
                                    ColorAnimation {
                                        to: "#4CAF50"
                                        duration: 500
                                    }
                                    ColorAnimation {
                                        to: "#81C784"
                                        duration: 500
                                    }
                                }
                            }
                            Text {
                                text: root.isActive ? (AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("active")) : (AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("inactive"))
                                font.pixelSize: 9
                                font.bold: true
                                color: root.isActive ? "#4CAF50" : "#888888"
                                Layout.alignment: Qt.AlignHCenter
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }

                    // Start/Stop Button
                    Rectangle {
                        id: startStopBtnRect
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.minimumWidth: 80
                        color: root.mode === 1 ? "#4a4a4a" : (root.isActive ? "#cc4444" : (tglBtnArea.containsMouse ? "#45a049" : "#4CAF50"))
                        radius: 8
                        opacity: root.mode === 1 ? 0.7 : 1.0

                        Text {
                            anchors.centerIn: parent
                            text: root.mode === 1 ? (AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("use_key")) : (root.isActive ? (AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("stop")) : (AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("start")))
                            font.pixelSize: 12
                            font.bold: true
                            color: "#ffffff"
                            elide: Text.ElideRight
                            width: startStopBtnRect.width - 10
                            horizontalAlignment: Text.AlignHCenter
                        }
                        MouseArea {
                            id: tglBtnArea
                            anchors.fill: parent
                            hoverEnabled: true
                            enabled: root.mode === 0
                            onClicked: AC.AutoClickerController.toggleProfile(root.profileId)
                        }
                    }

                    // Delete Button
                    Item {
                        Layout.preferredWidth: 72
                        Layout.fillHeight: true

                        Rectangle {
                            width: 38
                            height: 38
                            anchors.centerIn: parent
                            color: delBtnArea.containsMouse ? "#cc4444" : "#883333"
                            radius: 8

                            Text {
                                anchors.centerIn: parent
                                text: "✕"
                                font.pixelSize: 16
                                color: "#ffffff"
                            }
                            MouseArea {
                                id: delBtnArea
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: AC.AutoClickerController.removeProfile(root.profileId)
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: root
        function onFrequencyChanged() {
            if (!frequencyField.activeFocus)
                frequencyField.text = root.normalizedFrequencyText(root.frequency);
        }
        function onMaxFrequencyChanged() {
            if (!maxFrequencyField.activeFocus)
                maxFrequencyField.text = root.normalizedFrequencyText(root.maxFrequency);
        }
    }
}
