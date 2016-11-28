Stáhněte si instalaci VMWARE Playeru a připravený image (VMWare, Image - 800MB)
Nainstalujte si VMWARE player a z archívu rozbalte image s linuxem. V případě že chcete vyzkoušet vlastní distribuci, můžete si stáhnout pouze zdroják sctp klient/serveru (je nutná podpora v jádře)
Ve VMware playeru spusťte připravený image s linuxem
Spusťte terminál. V domovském adresáři uživatele student přejděte do adresáře sctp.
Otevřete zdrojové kódy SCTP klienta a serveru, podívejte se na rozdíly mezi TCP.
Příkazem make vše přeložte.
Spusťte Wireshark (všechna hesla nastavena na "student") a zapněte trace na rozhraní lo0
V jednom terminálu spusťte server, ve druhém klienta, analyzujte odchycené pakety
Zdrojový kód zmodifikujte tak, aby se oběma streamy paralelně přenesly dvě informace - jedním streamem soubor client.c, a druhým streamem soubor server.c, ideálně oba streamy kombinované do jednoho SCTP paketu.
Opět přeložte, spusťte a Wiresharkem proveďte trace přenosu souborů
Modifikované zdrojové kódy spolu s trace soubory zabalte do ZIP archívu a odevzdejte v IS (jméno souboru: login.zip, např. xnovak00.zip)
