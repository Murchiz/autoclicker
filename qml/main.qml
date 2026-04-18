import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import AutoClicker

Window {
    id: root
    visible: true
    width: 700
    height: 600
    minimumWidth: 600
    minimumHeight: 400
    title: translationManager.currentLanguage && translationManager.textFor("app_title")
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
                    text: translationManager.currentLanguage && translationManager.textFor("app_title")
                    font.pixelSize: 24
                    font.bold: true
                    color: "#ffffff"
                }

                Item { Layout.fillWidth: true }

                RowLayout {
                    spacing: 8

                    Text {
                        text: translationManager.currentLanguage && translationManager.textFor("language")
                        font.pixelSize: 13
                        color: "#888888"
                        Layout.alignment: Qt.AlignVCenter
                    }

                    ComboBox {
                        id: languageSelector
                        Layout.preferredWidth: 110
                        Layout.preferredHeight: 32
                        model: translationManager.availableLanguages
                        textRole: "name"
                        valueRole: "code"

                        Component.onCompleted: {
                            const index = indexOfValue(translationManager.currentLanguage)
                            if (index >= 0) {
                                currentIndex = index
                            }
                        }

                        onActivated: {
                            if (currentValue !== translationManager.currentLanguage) {
                                translationManager.setLanguage(currentValue)
                            }
                        }

                        delegate: ItemDelegate {
                            width: languageSelector.width
                            highlighted: languageSelector.highlightedIndex === index
                            contentItem: Text {
                                text: modelData.name
                                color: highlighted ? "#ffffff" : "#dddddd"
                                font.pixelSize: 13
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                color: highlighted ? "#3d5a80" : "#2f2f2f"
                            }
                        }

                        indicator: Canvas {
                            x: languageSelector.width - width - 10
                            y: (languageSelector.height - height) / 2
                            width: 12
                            height: 8
                            contextType: "2d"

                            Connections {
                                target: languageSelector
                                function onPressedChanged() { parent.requestPaint() }
                            }

                            onPaint: {
                                context.reset()
                                context.moveTo(0, 0)
                                context.lineTo(width, 0)
                                context.lineTo(width / 2, height)
                                context.closePath()
                                context.fillStyle = "#d8d8d8"
                                context.fill()
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
                                clip: true
                                implicitHeight: contentHeight
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
                            target: translationManager
                            function onCurrentLanguageChanged() {
                                const index = languageSelector.indexOfValue(translationManager.currentLanguage)
                                if (index >= 0 && languageSelector.currentIndex !== index) {
                                    languageSelector.currentIndex = index
                                }
                            }
                        }
                    }
                }

                Text {
                    text: translationManager.currentLanguage && translationManager.textFor("profiles_count").arg(autoClickerController.profileCount)
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
            visible: autoClickerController.isListeningForTarget || autoClickerController.isListeningForKeybind
            color: "#3d5a80"
            radius: 6

            Text {
                anchors.centerIn: parent
                text: autoClickerController.isListeningForTarget
                      ? (translationManager.currentLanguage && translationManager.textFor("press_target"))
                      : (translationManager.currentLanguage && translationManager.textFor("press_keybind"))
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
            model: autoClickerController
            spacing: 10

            delegate: ClickProfileItem {
                width: profileListView.width
                profileId: model.profileId
                targetButton: model.targetButton
                targetButtonCode: model.targetButtonCode
                keybind: model.keybind
                keybindCode: model.keybindCode
                frequency: model.frequency
                randomFrequencyEnabled: model.randomFrequencyEnabled
                maxFrequency: model.maxFrequency
                mode: model.mode
                isActive: model.isActive
            }

            // Empty state
            Text {
                anchors.centerIn: parent
                text: translationManager.currentLanguage && translationManager.textFor("empty_profiles")
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
                        NumberAnimation { duration: 200 }
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
                    text: translationManager.currentLanguage && translationManager.textFor("add_profile")
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
                    autoClickerController.addProfile()
                }
            }
        }

        // Footer info
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            color: "transparent"

            Text {
                anchors.centerIn: parent
                text: translationManager.currentLanguage && translationManager.textFor("footer_hint")
                font.pixelSize: 12
                color: "#666666"
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }
    }

    // Listening overlay
    Rectangle {
        anchors.fill: parent
        color: "#80000000"
        visible: autoClickerController.isListeningForTarget || autoClickerController.isListeningForKeybind
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
                    running: autoClickerController.isListeningForTarget || autoClickerController.isListeningForKeybind
                }

                Text {
                    text: translationManager.currentLanguage && translationManager.textFor("waiting_input")
                    font.pixelSize: 16
                    color: "#ffffff"
                    Layout.alignment: Qt.AlignHCenter
                }

                Button {
                    text: translationManager.currentLanguage && translationManager.textFor("cancel")
                    Layout.alignment: Qt.AlignHCenter
                    onClicked: {
                        autoClickerController.stopListening()
                    }
                    background: Rectangle {
                        color: parent.down ? "#cc4444" : (parent.hovered ? "#aa3333" : "#883333")
                        radius: 4
                    }
                    contentItem: Text {
                        text: parent.text
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
