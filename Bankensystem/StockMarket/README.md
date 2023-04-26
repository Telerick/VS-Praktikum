# StockMarket
## RTT
Die RTT wird beim Versenden einer Transaktion berechnet,
indem die Banken eine Bestätigung schicken, nachdem sie die Nachricht erhalten.
### RTT Version 1
In der ersten Version wurde die RTT berechnet, indem die Adresse der ersten Bank gespeichert wurde.
Sobald eine ACK Nachricht von der Bank kam, wurde die RTT berechnet.
Problematisch an dieser Version war, dass alle Banken ein ACK zurückgesendet haben, auch wenn es nur von einer Bank benötigt war.
Weiterhin war die Börse erst in der Lage die ACK Nachricht zu verwerten, wenn die Nachricht mit der Transaktion an alle Banken geschickt wurde.
Da die RTT nur mit der ersten Bank berechnet wurde, konnte es dazu kommen, dass erst viele weitere Nachrichten geschickt wurden.
Eine Messung der durchschnittlichen RTT ergab bei 100 Transaktionen eine Zeit von 4997 Mikrosekunden. (Macbook Air, x86)
### RTT Version 2
In der zweiten Version wurden die Fehler aus Version eins ausgebessert.
Die RTT wird nun anhand der letzten Nachricht, einer Transaktion berechnet.
So ist die Börse schneller in der Lage die ACK Nachricht zu erhalten und auszuwerten.
Des Weiteren wurde der Transaktion ein weiterer Parameter angehängt, dieser informiert die Bank, ob eine ACK Nachricht versendet werden soll.
Eine Messung der durchschnittlichen RTT ergab bei 100 Transaktionen eine Zeit von 2385 Mikrosekunden. (Macbook Air, x86)
### Andere Messungen