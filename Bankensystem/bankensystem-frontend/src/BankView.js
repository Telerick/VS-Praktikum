import React, { useState, useEffect } from "react";
import "./BankView.css";
import Wertpapier from "./Wertpapier";
import { handleStockTransaction } from "./Wertpapier";
import Header from "./Header";

const TCP_PORT = 8089; // TODO: to change

const generateRandomNumber = (min, max) => {
  const factor = max - min + 1;
  const random = Math.floor(Math.random() * factor) + min;
  return random;
};

const STOCKSDUMMIES = [
  { name: "Amazon", stueckzahlHaben: 100, aktuellerPreis: 160 },
  { name: "Apple", stueckzahlHaben: 200, aktuellerPreis: 190 },
  { name: "Google", stueckzahlHaben: 190, aktuellerPreis: 130 },
  { name: "Microsoft", stueckzahlHaben: 50, aktuellerPreis: 80 },
  { name: "Deutsche Telekom", stueckzahlHaben: 170, aktuellerPreis: 290 },
  { name: "Yahoo", stueckzahlHaben: 80, aktuellerPreis: 60 },
];

const BankView = (props) => {
  const [transactions, setTransactions] = useState([]);
  const [stocks, setStocks] = useState(STOCKSDUMMIES);
  const [enteredCashValue, setEnteredCashValue] = useState("");
  const [enteredCreditValue, setEnteredCreditValue] = useState("");
  const [bankValue, setBankValue] = useState(420190);
  const [cashReserves, setCashReserves] = useState("");
  const [outstandingLoans, setOutstandingLoans] = useState("");
  const [jsonUploaded, setJsonUploaded] = useState(false);

  useEffect(() => {
    const fetchData = async () => {
      try {
        const response = await fetch(
          `http://${props.bankName}:${TCP_PORT}/initalBankData`
        );
        const data = await response.json();
        setBankValue(data.bankValue);
        setCashReserves(data.cashReserves);
        setOutstandingLoans(data.outstandingLoans);
        setStocks(data.stocks);
      } catch (error) {
        console.error("Error fetching initial bank data:", error);
        // Hier können Sie den Fehler entsprechend behandeln, z.B. eine Fehlermeldung anzeigen
      }
    };
    fetchData();
  }, []);

  async function startCashOrLoansChangeRequest(bankName, requestBody) {
    const response = await fetch(`http://${bankName}:${TCP_PORT}/bankValues`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(requestBody),
    });
    const data = await response.json();
    return data;
  }

  const handleValueChanges = async (
    enteredValue,
    isCash,
    isDepositOrRepayment
  ) => {
    try {
      switch (isCash) {
        case true:
          if (isDepositOrRepayment) {
            const reqBody = {
              cashReserves: cashReserves + enteredValue,
              outstandingLoans: outstandingLoans,
            };
            const newCashReserves = startCashOrLoansChangeRequest(
              props.bankName,
              reqBody
            ).value;
            setCashReserves(newCashReserves);
          } else {
            if (cashReserves < enteredValue) {
              console.log(
                "Transaktion nicht möglich! Auszahlung würde Bargeldbestand übersteigen."
              );
              return;
            } else {
              const reqBody = {
                cashReserves: cashReserves + enteredValue,
                outstandingLoans: outstandingLoans,
              };
              const newCashReserves = startCashOrLoansChangeRequest(
                props.bankName,
                reqBody
              ).value;
              setCashReserves(newCashReserves);
            }
          }
          break;
        case false:
          if (isDepositOrRepayment) {
            const reqBody = {
              cashReserves: cashReserves,
              outstandingLoans: outstandingLoans - enteredValue,
            };
            const newOutstandingLoans = startCashOrLoansChangeRequest(
              props.bankName,
              reqBody
            ).value;
            setOutstandingLoans(newOutstandingLoans);
          } else {
            const reqBody = {
              cashReserves: cashReserves,
              outstandingLoans: outstandingLoans + enteredValue,
            };
            const newOutstandingLoans = startCashOrLoansChangeRequest(
              props.bankName,
              reqBody
            ).value;
            setOutstandingLoans(newOutstandingLoans);
          }
          break;
        default:
          console.log(`Unerwarteter Fehler`);
          return;
      }
    } catch (error) {
      console.error(`Error handle transaction:`, error);
    }
  };

  const cashValueChangeHandler = (event) => {
    setEnteredCashValue(event.target.value);
    console.log("cashValueChangeHandler: " + event.target.value);
  };

  const creditValueChangeHandler = (event) => {
    setEnteredCreditValue(event.target.value);
    console.log("creditValueChangeHandler: " + event.target.value);
  };

  const handleCashIncomeSubmit = (event) => {
    event.preventDefault();
    console.log(`EINZAHLUNG: Bargeldbestand um ${enteredCashValue}€ erhöht!`);
    // Code ausführen, um Einzahlung durchzuführen
    // POST-Methode
    setEnteredCashValue("");
  };

  const handleCashOutSubmit = (event) => {
    event.preventDefault();
    console.log(
      `AUSZAHLUNG: Bargeldbestand um ${enteredCashValue}€ verringert!`
    );
    // Code ausführen, um Auszahlung durchzuführen
    // POST-Methode
    setEnteredCashValue("");
  };

  const handleCreditBackSubmit = (event) => {
    event.preventDefault();
    console.log(
      `KREDITRÜCKZAHLUNG: Summe ausgezahlter Kreditbestände um ${enteredCreditValue}€ verringert!`
    );
    // Code ausführen, um Rückzahlung durchzuführen
    // POST-Methode
    setEnteredCreditValue("");
  };

  const handleCreditOutSubmit = (event) => {
    event.preventDefault();
    console.log(
      `KREDITAUSZAHLUNG: Summe ausgezahlter Kreditbestände um ${enteredCreditValue}€ erhöht!`
    );
    // Code ausführen, um Auszahlung durchzuführen
    // POST-Methode
    setEnteredCreditValue("");
  };

  // Funktion zum Auswerten der hochgeladenen Datei
  const handleFileUpload = (event) => {
    setJsonUploaded(true);
    const fileReader = new FileReader();
    fileReader.readAsText(event.target.files[0], "UTF-8");
    fileReader.onload = (e) => {
      const jsonData = JSON.parse(e.target.result);
      console.log(`JSONDATA.TRANSACTIONS: ${jsonData.transactions}`);
      setTransactions(jsonData.transactions);
    };
    console.log(`TRANSACTIONS: ${transactions}`);
  };

  const runTransaction = (transaction) => {
    switch (transaction.type) {
      case "cashOut": {
        handleValueChanges(transaction.amount, true, false);
        break;
      }

      case "cashBack": {
        handleValueChanges(transaction.amount, true, true);
        break;
      }

      case "creditOut": {
        handleValueChanges(transaction.amount, false, false);
        break;
      }

      case "creditBack": {
        handleValueChanges(transaction.amount, true, true);
        break;
      }

      case "buyStock": {
        const randomStocksIndex = generateRandomNumber(0, stocks.size());
        handleStockTransaction(
          props.bankName,
          stocks[randomStocksIndex],
          transaction.amount,
          "bought",
          stocks[randomStocksIndex].stueckzahlHaben
        );
        break;
      }

      case "sellStock": {
        const randomStocksIndex = generateRandomNumber(0, stocks.size());
        handleStockTransaction(
          props.bankName,
          stocks[randomStocksIndex],
          transaction.amount,
          "sold",
          stocks[randomStocksIndex].stueckzahlHaben
        );
        break;
      }

      default: {
        console.log("Unknown transaction type");
      }
    }
  };

  const processTransactions = () => {
    transactions.forEach((transaction) => {
      runTransaction(transaction);
    });
  };

  const startTransactionSimulation = () => {
    console.log(transactions);
    setJsonUploaded(false);
    //processTransactions();
  };

  return (
    <div className="container">
      <Header />
      <div className="bank-info-container">
        <h2>
          Gesamtwert {props.bankName}: {bankValue} €
        </h2>
      </div>

      <div className="transaction-container">
        <h3>Bargeld</h3>
        <form>
          <label htmlFor="cash">
            Betrag (€):
            <input
              id="cash"
              type="number"
              value={enteredCashValue}
              onChange={cashValueChangeHandler}
            />
          </label>
          <button type="submit" onClick={handleCashIncomeSubmit}>
            Einzahlen
          </button>
          <button type="submit" onClick={handleCashOutSubmit}>
            Auszahlen
          </button>
        </form>
      </div>

      <div className="credit-container">
        <h3>Kredite</h3>
        <form>
          <label htmlFor="credit">
            Betrag (€):
            <input
              id="credit"
              type="number"
              value={enteredCreditValue}
              onChange={creditValueChangeHandler}
            />
          </label>
          <button type="submit" onClick={handleCreditBackSubmit}>
            Rückzahlung
          </button>
          <button type="submit" onClick={handleCreditOutSubmit}>
            Auszahlung
          </button>
        </form>
      </div>

      <div className="securities-container">
        <h3>Wertpapiere</h3>
        <div>
          {stocks.map((stock) => (
            <Wertpapier
              bankName={props.bankName}
              key={stock.name}
              name={stock.name}
              stueckzahlHaben={stock.stueckzahlHaben}
              aktuellerPreis={stock.aktuellerPreis}
            />
          ))}
        </div>
      </div>

      <div className="file-upload-container">
        <h2>Upload einer Transaktionsdatei zur Simulation</h2>
        {!jsonUploaded ? (
          <>
            <label htmlFor="file-upload-input">
              Transaktionsdatei hochladen
            </label>
            <input
              type="file"
              id="file-upload-input"
              accept=".json"
              onChange={handleFileUpload}
            />
          </>
        ) : (
          <>
            <label htmlFor="file-upload-start">
              Transaktionssimulation starten
            </label>
            <button
              type="button"
              id="file-upload-start"
              onClick={startTransactionSimulation}
            />
          </>
        )}
      </div>

      <div className="footer">
        © {new Date().getFullYear()} Toggo Transactions Inc.
      </div>
    </div>
  );
};

export default BankView;
