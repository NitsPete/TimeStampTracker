import sys
import uno
import os
from pathlib import Path
from com.sun.star.task import ErrorCodeIOException

from constants import *
from functionLogger import log_function

@log_function
def connect2libreOffice():
    local_context = uno.getComponentContext()
    resolver = local_context.ServiceManager.createInstanceWithContext(
        "com.sun.star.bridge.UnoUrlResolver", local_context
    )
    context = resolver.resolve("uno:socket,host=localhost,port=2002;urp;StarOffice.ComponentContext")
    return context

@log_function
def openDocument(path: Path):
    context = connect2libreOffice()
    desktop = context.ServiceManager.createInstanceWithContext(
        "com.sun.star.frame.Desktop", context
    )
    file_url = uno.systemPathToFileUrl(str(path))
    document = desktop.loadComponentFromURL(file_url, "_blank", 0, ())
    return document

@log_function
def getSheetData(document):
    sheets = document.Sheets
    sheet = sheets.getByIndex(0)  # Sheet 0 is always the newest sheet
    column_count = sheet.getColumns().getCount()

    for row_index in range(ROW_FIRST_DATA, MAX_NAME_ROWS + 1):
        for col_index in range(column_count):
            cell = sheet.getCellByPosition(col_index, row_index)

            if( (not cell.getString()) and (col_index != COLUMN_FIRST_CHECK_IN)):
                break

            # First column in loop
            if(col_index == COLUMN_NAME):
                print(row_index - HEADER_ROWS)
                print(cell.CellBackColor)

            print(cell.getString())
        print("\r") # Indicates end of employee infos (This value is print as "\r\n")

@log_function
def main_readData(): 
    argc = len(sys.argv)
    if argc != 2:
        print("Wrong parameter count!")
        return 

    path = Path(sys.argv[1])
    os.makedirs(os.path.dirname(path), exist_ok=True)

    document = openDocument(path)

    getSheetData(document)

    document.close(True)

main_readData()