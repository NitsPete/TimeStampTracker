import sys
import uno
from pathlib import Path
from com.sun.star.task import ErrorCodeIOException

HEADER_ROWS = 1
FIRST_CHECKIN_COL = 3

def connect2libreOffice():
    local_context = uno.getComponentContext()
    resolver = local_context.ServiceManager.createInstanceWithContext(
        "com.sun.star.bridge.UnoUrlResolver", local_context
    )
    context = resolver.resolve("uno:socket,host=localhost,port=2002;urp;StarOffice.ComponentContext")
    return context

def openDocument(path: Path):
    context = connect2libreOffice()
    desktop = context.ServiceManager.createInstanceWithContext(
        "com.sun.star.frame.Desktop", context
    )
    file_url = uno.systemPathToFileUrl(str(path))
    document = desktop.loadComponentFromURL(file_url, "_blank", 0, ())
    return document

def getRowInfos(document, row_number):
    sheets = document.Sheets
    sheet = sheets.getByIndex(0)  # Sheet 0 is always the newest sheet
    column_count = sheet.getColumns().getCount()

    for col_index in range(column_count):
        cell = sheet.getCellByPosition(col_index, row_number + HEADER_ROWS)
        if(col_index == 0):
            print(cell.CellBackColor)

        if(cell.getString() == "" and col_index != FIRST_CHECKIN_COL):
            break
        print(cell.getString())

def main(): 
    argc = len(sys.argv)
    if argc != 3:
        print("Wrong parameter count!")
        return 

    path = Path(sys.argv[1])
    row_number = int(sys.argv[2])

    document = openDocument(path)

    getRowInfos(document, row_number)

    try:
        document.store() # If document already exist you can use store() instead of storeAsURL(url, properties)
    except ErrorCodeIOException:
        print("Unable to save file!")
        document.close(True)
        return -1

    document.close(True)

main()