pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import AutoClicker as AC

Window {
    id: root
    visible: true
    width: 700
    height: 600
    minimumWidth: 600
    minimumHeight: 400
    title: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("app_title")
    color: "#1e1e1e"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // Header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 72
            color: "#2d2d2d"
            radius: 8

            RowLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 12

                Text {
                    text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("app_title")
                    font.pixelSize: 24
                    font.bold: true
                    color: "#ffffff"
                }

                Item {
                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: 8

                    Text {
                        text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("language")
                        font.pixelSize: 13
                        color: "#888888"
                        Layout.alignment: Qt.AlignVCenter
                    }

                    ComboBox {
                        id: languageSelector
                        Layout.preferredWidth: 110
                        Layout.preferredHeight: 32
                        model: AC.TranslationManager.availableLanguages
                        textRole: "name"
                        valueRole: "code"

                        Component.onCompleted: {
                            const index = languageSelector.indexOfValue(AC.TranslationManager.currentLanguage);
                            if (index >= 0) {
                                languageSelector.currentIndex = index;
                            }
                        }

                        onActivated: {
                            if (languageSelector.currentValue !== AC.TranslationManager.currentLanguage) {
                                AC.TranslationManager.setLanguage(languageSelector.currentValue);
                            }
                        }

                        delegate: ItemDelegate {
                            id: languageDelegate
                            required property int index
                            required property var modelData
                            width: languageSelector.width
                            highlighted: languageSelector.highlightedIndex === languageDelegate.index
                            contentItem: Text {
                                text: languageDelegate.modelData["name"] // Using bracket notation avoids qmllint 'var property' warnings
                                color: languageDelegate.highlighted ? "#ffffff" : "#dddddd"
                                font.pixelSize: 13
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                color: languageDelegate.highlighted ? "#3d5a80" : "#2f2f2f"
                            }
                        }

                        indicator: Canvas {
                            id: comboIndicator
                            x: languageSelector.width - comboIndicator.width - 10
                            y: (languageSelector.height - comboIndicator.height) / 2
                            width: 12
                            height: 8
                            contextType: "2d"

                            Connections {
                                target: languageSelector
                                function onPressedChanged() {
                                    comboIndicator.requestPaint();
                                }
                            }

                            onPaint: {
                                var ctx = comboIndicator.getContext("2d");
                                ctx.reset();
                                ctx.moveTo(0, 0);
                                ctx.lineTo(comboIndicator.width, 0);
                                ctx.lineTo(comboIndicator.width / 2, comboIndicator.height);
                                ctx.closePath();
                                ctx.fillStyle = "#d8d8d8";
                                ctx.fill();
                            }
                        }

                        contentItem: Text {
                            leftPadding: 10
                            rightPadding: languageSelector.indicator.width + 16
                            text: languageSelector.displayText
                            font.pixelSize: 13
                            color: "#f0f0f0"
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }

                        background: Rectangle {
                            radius: 6
                            color: "#3a3a3a"
                            border.width: languageSelector.visualFocus ? 2 : 1
                            border.color: languageSelector.visualFocus ? "#5a9fd4" : "#555555"
                        }

                        popup: Popup {
                            y: languageSelector.height + 4
                            width: languageSelector.width
                            padding: 1

                            contentItem: ListView {
                                id: popupListView
                                clip: true
                                implicitHeight: popupListView.contentHeight
                                model: languageSelector.popup.visible ? languageSelector.delegateModel : null
                                currentIndex: languageSelector.highlightedIndex
                            }

                            background: Rectangle {
                                radius: 6
                                color: "#2f2f2f"
                                border.width: 1
                                border.color: "#555555"
                            }
                        }

                        Connections {
                            target: AC.TranslationManager
                            function onCurrentLanguageChanged() {
                                const index = languageSelector.indexOfValue(AC.TranslationManager.currentLanguage);
                                if (index >= 0 && languageSelector.currentIndex !== index) {
                                    languageSelector.currentIndex = index;
                                }
                            }
                        }
                    }
                }

                Text {
                    text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("profiles_count").arg(AC.AutoClickerController.profileCount)
                    font.pixelSize: 14
                    color: "#888888"
                    Layout.alignment: Qt.AlignVCenter
                }
            }
        }

        // Instructions banner
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            visible: AC.AutoClickerController.isListeningForTarget || AC.AutoClickerController.isListeningForKeybind
            color: "#3d5a80"
            radius: 6

            Text {
                anchors.centerIn: parent
                text: AC.AutoClickerController.isListeningForTarget ? (AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("press_target")) : (AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("press_keybind"))
                font.pixelSize: 14
                color: "#ffffff"
            }
        }

        // Profile List
        ListView {
            id: profileListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: AC.AutoClickerController
            spacing: 10

            // The list view automatically injects the roles directly into the 'required' properties!
            delegate: ClickProfileItem {
                width: profileListView.width
            }

            // Empty state
            Text {
                anchors.centerIn: parent
                text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("empty_profiles")
                font.pixelSize: 16
                color: "#666666"
                horizontalAlignment: Text.AlignHCenter
                visible: profileListView.count === 0
            }

            ScrollBar.vertical: ScrollBar {
                id: verticalScrollBar
                width: 12
                active: true
                policy: ScrollBar.AsNeeded

                contentItem: Rectangle {
                    implicitWidth: 6
                    implicitHeight: 100
                    radius: 3
                    color: verticalScrollBar.pressed ? "#888888" : (verticalScrollBar.hovered ? "#666666" : "#444444")
                    opacity: verticalScrollBar.active ? 1.0 : 0
                    Behavior on opacity {
                        NumberAnimation {
                            duration: 200
                        }
                    }
                }

                background: Rectangle {
                    implicitWidth: 12
                    color: "transparent"
                }
            }
        }

        // Add Profile Button
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: addProfileArea.containsMouse ? "#4a7c59" : "#3d5a3d"
            radius: 8

            RowLayout {
                anchors.centerIn: parent
                spacing: 10

                Text {
                    text: "+"
                    font.pixelSize: 28
                    font.bold: true
                    color: "#ffffff"
                }

                Text {
                    text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("add_profile")
                    font.pixelSize: 16
                    color: "#ffffff"
                }
            }

            MouseArea {
                id: addProfileArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    AC.AutoClickerController.addProfile();
                }
            }
        }

        // Footer info
        Rectangle {
            id: footerRect
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            color: "transparent"

            Text {
                anchors.centerIn: parent
                text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("footer_hint")
                font.pixelSize: 12
                color: "#666666"
                width: footerRect.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }
    }

    // Listening overlay
    Rectangle {
        anchors.fill: parent
        color: "#80000000"
        visible: AC.AutoClickerController.isListeningForTarget || AC.AutoClickerController.isListeningForKeybind
        z: 100

        MouseArea {
            anchors.fill: parent
        }

        Rectangle {
            anchors.centerIn: parent
            width: 300
            implicitHeight: contentColumn.implicitHeight + 40
            color: "#2d2d2d"
            radius: 12
            border.color: "#3d5a80"
            border.width: 2

            ColumnLayout {
                id: contentColumn
                anchors.fill: parent
                anchors.margins: 20
                spacing: 16

                BusyIndicator {
                    Layout.alignment: Qt.AlignHCenter
                    running: AC.AutoClickerController.isListeningForTarget || AC.AutoClickerController.isListeningForKeybind
                }

                Text {
                    text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("waiting_input")
                    font.pixelSize: 16
                    color: "#ffffff"
                    Layout.alignment: Qt.AlignHCenter
                }

                Button {
                    id: cancelButton
                    text: AC.TranslationManager.currentLanguage && AC.TranslationManager.textFor("cancel")
                    Layout.alignment: Qt.AlignHCenter
                    onClicked: {
                        AC.AutoClickerController.stopListening();
                    }
                    background: Rectangle {
                        color: cancelButton.down ? "#cc4444" : (cancelButton.hovered ? "#aa3333" : "#883333")
                        radius: 4
                    }
                    contentItem: Text {
                        text: cancelButton.text
                        font.pixelSize: 14
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
}
