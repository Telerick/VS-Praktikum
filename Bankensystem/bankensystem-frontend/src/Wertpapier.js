import React from "react";
import "./Wertpapier.css";

function Wertpapier(props) {
  return (
    <div className="wertpapier-container">
      <h3>{props.name}</h3>
      <p>Stückzahl: {props.stueckzahlHaben}</p>
      <p>Preis: {props.aktuellerPreis} €</p>
      <input type="number" min={0} max={props.stueckzahlHaben} />
      <button>Kaufen</button>
      <button>Verkaufen</button>
    </div>
  );
}

export default Wertpapier;
