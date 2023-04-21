import React, { useState, useEffect } from "react";
import "./BankView.css";
import Wertpapier from "./Wertpapier";
import Header from "./Header";

const securities = [
  { name: "Amazon", stueckzahlHaben: 100, aktuellerPreis: 160 },
  { name: "Apple", stueckzahlHaben: 200, aktuellerPreis: 190 },
  { name: "Google", stueckzahlHaben: 190, aktuellerPreis: 130 },
  { name: "Microsoft", stueckzahlHaben: 50, aktuellerPreis: 80 },
  { name: "Deutsche Telekom", stueckzahlHaben: 170, aktuellerPreis: 290 },
  { name: "Yahoo", stueckzahlHaben: 80, aktuellerPreis: 60 },
];

const BankView = ({ bankName }) => {
  const [transactions, setTransactions] = useState([]);
  /*
  const [bankValue, setBankValue] = useState(null);
  const [cashReserves, setBankValue] = useState(null);
  const [outstandingLoans, setBankValue] = useState(null);
  

  useEffect(() => {
    const fetchData = async () => {
      const response = await fetch("https://example.com/api/value");
      const data = await response.json();
      setBankValue(data);
    };
    fetchData();
  }, []);
  */

  // Funktion zum Auswerten der hochgeladenen Datei
  const handleFileUpload = (event) => {
    const fileReader = new FileReader();
    fileReader.readAsText(event.target.files[0], "UTF-8");
    fileReader.onload = (e) => {
      const jsonData = JSON.parse(e.target.result);
      setTransactions(jsonData.transactions);
    };
  };

  return (
    <div className="container">
      <Header />
      <div className="bank-info-container">
        <h2>Gesamtwert {bankName}: €100.000</h2>
      </div>

      <div className="transaction-container">
        <h3>Bargeld</h3>
        <form>
          <label>
            Betrag (€):
            <input type="number" />
          </label>
          <button type="submit">Einzahlen</button>
          <button type="submit">Auszahlen</button>
        </form>
      </div>

      <div className="credit-container">
        <h3>Kredite</h3>
        <form>
          <label>
            Betrag (€):
            <input type="number" />
          </label>
          <button type="submit">Rückzahlung</button>
          <button type="submit">Auszahlung</button>
        </form>
      </div>

      <div className="securities-container">
        <h3>Wertpapiere</h3>
        <div>
          {securities.map((security) => (
            <Wertpapier
              key={security.name}
              name={security.name}
              stueckzahlHaben={security.stueckzahlHaben}
              aktuellerPreis={security.aktuellerPreis}
            />
          ))}
        </div>
      </div>

      <div className="file-upload-container">
        <h2>Upload einer Transaktionsdatei zur Simulation</h2>
        <label htmlFor="file-upload-input">Transaktionsdatei hochladen</label>
        <input
          type="file"
          id="file-upload-input"
          accept=".json"
          onChange={handleFileUpload}
        />
      </div>

      <div className="footer">
        © {new Date().getFullYear()} Toggo Transactions Inc.
      </div>
    </div>
  );
};

export default BankView;
