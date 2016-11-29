Spusťte terminál. V domovském adresáři uživatele student přejděte do adresáře sctp.
Otevřete zdrojové kódy SCTP klienta a serveru, podívejte se na rozdíly mezi TCP.
Příkazem make vše přeložte.
Spusťte Wireshark (všechna hesla nastavena na "student") a zapněte trace na rozhraní lo0
V jednom terminálu spusťte server, ve druhém klienta, analyzujte odchycené pakety
Zdrojový kód zmodifikujte tak, aby se oběma streamy paralelně přenesly dvě informace - jedním streamem soubor client.c, a druhým streamem soubor server.c, ideálně oba streamy kombinované do jednoho SCTP paketu.
Opět přeložte, spusťte a Wiresharkem proveďte trace přenosu souborů
Modifikované zdrojové kódy spolu s trace soubory zabalte do ZIP archívu a odevzdejte v IS (jméno souboru: login.zip, např. xnovak00.zip)
