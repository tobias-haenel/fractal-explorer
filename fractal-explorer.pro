TEMPLATE = subdirs

SUBDIRS += \
    logic \
    logic \
    ui \
    ui

ui.depends = logic
