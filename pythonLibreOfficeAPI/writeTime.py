import sys
import uno
from pathlib import Path
from com.sun.star.task import ErrorCodeIOException

from constants import *

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

    for i in range(0, len(row_number)):       
        # Check if user is allowed to self check in
        skipFirstCheckInTime = False
        cell = sheet.getCellByPosition(0, row_number[i] + HEADER_ROWS)   
        if cell.CellBackColor == -1:
            skipFirstCheckInTime = True

        # Find first cell to write time
        col_number = COLUMN_FIRST_CHECK_IN
        if not isCheckInTime[i]:
            col_number += 1
        elif skipFirstCheckInTime:
            col_number += 2

        # Write time to first empty cell
        while True:
            cell = sheet.getCellByPosition(col_number, row_number[i] + HEADER_ROWS)

            if cell.Type == uno.Enum("com.sun.star.table.CellContentType", "EMPTY"):
                cell.setString(time[i])
                break

            col_number += 2

def main(): 
    VAR_PARAM_COUNT = 3
    FIX_PARAM_COUNT = 2

    argc = len(sys.argv)
    if (argc % VAR_PARAM_COUNT) != FIX_PARAM_COUNT:
        print("Wrong parameter count!")
        return 

    path = Path(sys.argv[1])

    row_number = []
    time = []
    isCheckInTime = []

    maxRows = int((argc-FIX_PARAM_COUNT) / VAR_PARAM_COUNT)

    for i in range(0, maxRows):
        row_number.append(int(sys.argv[2 + VAR_PARAM_COUNT * i])) 
        time.append(sys.argv[3 + VAR_PARAM_COUNT * i])
        if sys.argv[4 + VAR_PARAM_COUNT * i] == "0":
            isCheckInTime.append(True)
        else:
            isCheckInTime.append(False)

    document = openDocument(path)

    writeTime(document, row_number, time, isCheckInTime)

    try:
        document.store() # If document already exist you can use store() instead of storeAsURL(url, properties)
    except ErrorCodeIOException:
        print("Unable to save file!")
        document.close(True)
        sys.exit(FAILED_TO_SAVE_FILE)

    document.close(True)

main()