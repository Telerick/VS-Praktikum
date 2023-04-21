import React from "react";
import "./Header.css";

const Header = () => {
  return (
    <div className="header">
      <div className="text">
        <h1>Toggo Transactions</h1>
        <p>Experience seamless banking with Burda and Toggo Transactions!</p>
      </div>
      <img
        src={require("./assets/Toggo_logo.png")}
        alt="Logo"
        className="logo"
      />
    </div>
  );
};

export default Header;
