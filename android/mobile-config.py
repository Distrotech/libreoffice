#!/usr/bin/python
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# this tool rips out configuration pieces that are not useful for
# a mobile viewer / editing application without a full UI.
#
# ideally the postprocess/ makefile would cope with this but its
# already over-complicated by rampant conditionals.

import sys
import xml.etree.ElementTree as ET


main_xcd_discard = [
    'org.openoffice.Office/TableWizard', # huge

    'org.openoffice.Office/WebWizard',
    'org.openoffice.Office.DataAccess/Drivers', # no database
    'org.openoffice.Office/Addons', # no addons

    # no conventional UI; reverse sorted by size
    'org.openoffice.Office.UI/GenericCommands',
    'org.openoffice.Office/Accelerators',
    'org.openoffice.Office/UI',

    'org.openoffice.Office.UI/DrawImpressCommands',
    'org.openoffice.Office.UI/Sidebar',
    'org.openoffice.Office.UI/ChartCommands',
    'org.openoffice.Office.UI/DbuCommands',
    'org.openoffice.Office.UI/Controller',
    'org.openoffice.Office/UI',
    'org.openoffice.Office.UI/StartModuleCommands',
    'org.openoffice.Office.UI/BasicIDEWindowState',
    'org.openoffice.Office.UI/GenericCategories',
    'org.openoffice.Office.UI/ChartWindowState',
    'org.openoffice.Office.UI/Factories',
    'org.openoffice.Office.UI/BaseWindowState',
    'org.openoffice.Office.UI/BasicIDECommands',
    'org.openoffice.Office.UI/Sidebar',
    'org.openoffice.Office.UI/BibliographyCommands',
    'org.openoffice.Office.UI/DbQueryWindowState',
    'org.openoffice.Office.UI/WindowState',
    'org.openoffice.Office.UI/Controller',
    'org.openoffice.Office.UI/DbRelationWindowState',
    'org.openoffice.Office.UI/DbTableWindowState',
    'org.openoffice.Office.UI/DbTableDataWindowState',
    'org.openoffice.Office.UI/DbBrowserWindowState',
    'org.openoffice.Office.UI/WindowContentFactories',
    'org.openoffice.Office.UI/Factories',
    'org.openoffice.Office.UI/StartModuleWindowState',
    'org.openoffice.Office.UI/GlobalSettings',
    'org.openoffice.Office.UI/Commands',
    'org.openoffice.Office.UI/BibliographyCommands',
    'org.openoffice.Office.UI/StartModuleCommands',
    'org.openoffice.Office.UI/DrawImpressCommands',
    'org.openoffice.Office.UI/BasicIDECommands',
    'org.openoffice.Office.UI/GenericCommands',
    'org.openoffice.Office.UI/ChartCommands',
    'org.openoffice.Office.UI/DbuCommands',
    'org.openoffice.Office.UI/BibliographyWindowState',
    'org.openoffice.Office.UI/StartModuleWindowState',
    'org.openoffice.Office.UI/DbTableDataWindowState',
    'org.openoffice.Office.UI/DbRelationWindowState',
    'org.openoffice.Office.UI/DbBrowserWindowState',
    'org.openoffice.Office.UI/BasicIDEWindowState',
    'org.openoffice.Office.UI/DbTableWindowState',
    'org.openoffice.Office.UI/DbQueryWindowState',
    'org.openoffice.Office.UI/ChartWindowState',
    'org.openoffice.Office.UI/BaseWindowState',
    'org.openoffice.Office.UI/GenericCategories',
    'org.openoffice.Office.UI/Category',
    ]


if __name__ == '__main__':
    tree = ET.parse(sys.argv[1])
    root = tree.getroot()

    print "Foo\n"
    saved = 0
    total = 0
    for child in root:
        section = child.attrib['{http://openoffice.org/2001/registry}name']
        package = child.attrib['{http://openoffice.org/2001/registry}package']
        size = len(ET.tostring(child));
        total = total + size
        key = '%s/%s' % (package, section)
        if key in main_xcd_discard:
            root.remove(child)
            print 'removed %s - saving %d' % (key, size)
            saved = saved + size

    print "saved %d of %d bytes: %2.f%%" % (saved, total, saved*100.0/total)
    tree.write(sys.argv[2], 'UTF-8', True)

