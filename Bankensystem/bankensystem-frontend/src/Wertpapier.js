import React, { useState } from "react";
import "./Wertpapier.css";

const TCP_PORT = 8089; // TODO: to change

export async function stockTransactionRequest(
  bankName,
  amount,
  stockName,
  soldOrBought
) {
  const response = await fetch(`http://${bankName}:${TCP_PORT}/updateStock`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({
      stockName: stockName,
      numberOfStocks: amount,
    }),
  });
  const data = await response.json();
  console.log(`Successfully ${soldOrBought} ${amount} stocks of ${stockName}.`);
  return data.stockAmount;
}

export const handleStockTransaction = async (
  bankName,
  stockName,
  stockDifference,
  soldOrBought,
  oldNumberOfStocks
) => {
  try {
    switch (soldOrBought) {
      case "bought":
        return stockTransactionRequest(
          bankName,
          oldNumberOfStocks + stockDifference,
          stockName,
          soldOrBought
        );

      case "sold":
        return stockTransactionRequest(
          bankName,
          oldNumberOfStocks - stockDifference,
          stockName,
          soldOrBought
        );

      default:
        console.log("Unknown stock transaction");
    }
  } catch (error) {
    console.error(`Error buying stocks of ${stockName}:`, error);
    return -1;
  }
};

const Wertpapier = (props) => {
  const [numberOfStocks, setNumberOfStocks] = useState(props.stueckzahlHaben);
  const [enteredValue, setEnteredValue] = useState("");

  const handleOnSellClicked = () => {
    console.log(
      `WERTPAPIERVERKAUF: ${enteredValue} Wertpapiere von ${props.name} verkauft.`
    );
    // const newNumberOfStocks = handleStockTransaction(
    //   props,
    //   enteredValue,
    //   "sold",
    //   numberOfStocks
    // );
    // if (newNumberOfStocks >= 0) {
    //   setNumberOfStocks(newNumberOfStocks);
    // }
    setEnteredValue("");
  };

  const handleOnBuyClicked = () => {
    console.log(
      `WERTPAPIERKAUF: ${enteredValue} Wertpapiere von ${props.name} gekauft.`
    );
    // const newNumberOfStocks = handleStockTransaction(
    //   props,
    //   enteredValue,
    //   "bought",
    //   numberOfStocks
    // );
    // if (newNumberOfStocks >= 0) {
    //   setNumberOfStocks(newNumberOfStocks);
    // }
    setEnteredValue("");
  };

  const valueChangeHandler = (event) => {
    setEnteredValue(event.target.value);
    console.log("valueChangeHandler: " + event.target.value);
  };

  return (
    <div className="wertpapier-container">
      <h3>{props.name}</h3>
      <p>Stückzahl: {numberOfStocks}</p>
      <p>Preis: {props.aktuellerPreis} €</p>
      <input
        type="number"
        onChange={valueChangeHandler}
        min={0}
        max={numberOfStocks}
        value={enteredValue}
      />
      <button onClick={handleOnBuyClicked}>Kaufen</button>
      <button onClick={handleOnSellClicked}>Verkaufen</button>
    </div>
  );
};

export default Wertpapier;
