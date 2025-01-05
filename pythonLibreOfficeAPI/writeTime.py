import sys
import uno
from pathlib import Path

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

def writeTime(document, row_number, time, isCheckInTime):
    sheets = document.Sheets
    sheet = sheets.getByIndex(0)  # Sheet 0 is always the newest sheet

    # Check if user is allowed to self check in
    skipFirstCheckInTime = False
    cell = sheet.getCellByPosition(0, row_number + HEADER_ROWS)   
    if cell.CellBackColor == -1:
        skipFirstCheckInTime = True

    col_number = FIRST_CHECKIN_COL
    if not isCheckInTime:
        col_number += 1
    elif skipFirstCheckInTime:
        col_number += 2

    while True:
        cell = sheet.getCellByPosition(col_number, row_number + HEADER_ROWS)

        if cell.Type == uno.Enum("com.sun.star.table.CellContentType", "EMPTY"):
            cell.setString(time)
            break

        col_number += 2

    
    timeSeason = sheet.getCellByPosition(1, row_number + HEADER_ROWS).getString()
    timeDay = sheet.getCellByPosition(2, row_number + HEADER_ROWS).getString()

    print(timeSeason)
    print(timeDay)

def main(): 
    argc = len(sys.argv)
    if argc != 5:
        print("Wrong parameter count!")
        return 

    path = Path(sys.argv[1])
    row_number = int(sys.argv[2])
    time = sys.argv[3]
    isCheckInTime = False
    if sys.argv[4] == "0":
        isCheckInTime = True

    document = openDocument(path)

    writeTime(document, row_number, time, isCheckInTime)

    document.store() # If document already exist you can use store() instead of storeAsURL(url, properties)

    document.close(True)

main()