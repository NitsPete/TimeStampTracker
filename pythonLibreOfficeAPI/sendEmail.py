import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.application import MIMEApplication
from datetime import datetime

import os
import sys
from pathlib import Path
from constants import *

def main_sendEmail():
    argc = len(sys.argv)
    if argc != 2:
        print("Wrong parameter count!")
        return 

    path = Path(sys.argv[1])
    os.makedirs(os.path.dirname(path), exist_ok=True)

    smtp_server = "mail.gmx.net"
    smtp_port = 465

    email_receiver = EMAIL_SENDER

    subject = "Backup: " + datetime.now().strftime("%Y.%m.%d")

    msg = MIMEMultipart()
    msg["From"] = EMAIL_SENDER
    msg["To"] = email_receiver
    msg["Subject"] = subject

    filename = str(path)   # Pfad zur Datei

    with open(filename, "rb") as f:
        print(f)
        part = MIMEApplication(f.read(), Name=filename)
        part["Content-Disposition"] = f'attachment; filename="{filename}"'
        msg.attach(part)

    with smtplib.SMTP_SSL(smtp_server, smtp_port) as server:
        server.login(EMAIL_SENDER, EMAIL_PASSWORD)
        server.sendmail(EMAIL_SENDER, email_receiver, msg.as_string())

    print("Mail wurde gesendet.")

main_sendEmail()