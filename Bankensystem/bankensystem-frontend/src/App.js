import "./App.css";
import React, { useState } from "react";
import BankList from "./BankList";
import BankView from "./BankView";

function App() {
  const [selectedBank, setSelectedBank] = useState(null);

  const handleBankClick = (bankName) => {
    setSelectedBank(bankName);
  };

  return (
    <div className="App">
      {selectedBank ? (
        <BankView bankName={selectedBank} />
      ) : (
        <BankList onBankClick={handleBankClick} />
      )}
    </div>
  );
}

export default App;
