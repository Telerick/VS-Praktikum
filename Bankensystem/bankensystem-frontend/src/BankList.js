import React from "react";
//import banks from "./banks.json";
import classes from "./BankList.module.css";
import Header from "./Header";

const banks = [
  "RickBank",
  "LionBank",
  "NorbiBank",
  "DonLangenBank",
  "Commerzbank",
  "Deutsche Bank",
  "Credit Suisse",
];

const BankList = (props) => {
  return (
    <div className={classes["bank-list-container"]}>
      <Header />
      <div className={classes["bank-list-inner"]}>
        <div className={classes["bank-list-logo-container"]}>
          <img
            src={require("./assets/boerse.webp")}
            alt="Boerse"
            className={classes["bank-list-boerse"]}
          />
        </div>
        <p className={classes["bank-list-header-p"]}>
          Choose your financial institute:
        </p>
        <div className={classes["bank-list"]}>
          {banks.map((bank) => (
            <button
              key={bank}
              className={classes["bank-list-item"]}
              onClick={() => props.onBankClick(bank)}
            >
              {bank}
            </button>
          ))}
        </div>
      </div>
      <div className={classes["bank-list-footer"]}>
        © {new Date().getFullYear()} Toggo Transactions Inc.
      </div>
    </div>
  );
};

export default BankList;
