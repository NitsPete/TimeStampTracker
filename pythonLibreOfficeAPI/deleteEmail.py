import imaplib
from datetime import datetime, timedelta

from constants import *

imap_server = "imap.gmx.net"

# Verbindung
mail = imaplib.IMAP4_SSL(imap_server)
mail.login(EMAIL_SENDER, EMAIL_PASSWORD)
mail.select("INBOX")

# Datum: 30 Tage zurück
date_cutoff = (datetime.now() - timedelta(days=1)).strftime("%d-%b-%Y")

# Nur Mails vor diesem Datum suchen
status, messages = mail.search(None, f'BEFORE {date_cutoff}')

if status != "OK":
    print("Fehler bei Suche")
    exit()

mail_ids = messages[0].split()

print(f"Gefunden: {len(mail_ids)} alte Mails")

for mail_id in mail_ids:
    mail.store(mail_id, "+FLAGS", "\\Deleted")

# endgültig löschen
mail.expunge()

mail.logout()

print("Alte Mails gelöscht")