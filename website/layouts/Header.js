"use client";
import Link from "next/link";
import { useEffect, useState } from "react";

const Header = ({ dark }) => {
  const [toggle, setToggle] = useState(false);
  const onClick = (e) => {
    e.preventDefault();
    setToggle(!toggle);
  };

  const [pathname, setPathname] = useState("home");
  useEffect(() => {
    setPathname(window.location.pathname);
  }, []);

  return (
    <header className={toggle ? "active" : ""}>
      <div className="head-top">
        <a href="#" className="menu-btn" onClick={(e) => onClick(e)}>
          <span />
        </a>
        <div className="logo">
          <Link href="/">
            <img src={`/images/logo.png`} alt="logo" />
          </Link>
        </div>
        <div className="top-menu">
          <ul>
            <li className={pathname.includes("index") ? "active" : ""}>
              <Link href="/" className={`lnk`}>
                Home
              </Link>
            </li>
            <li className={pathname.includes("feature") ? "active" : ""}>
              <Link href="/feature" className={`lnk`}>
                Feature
              </Link>
            </li>
            <li className={pathname.includes("install") ? "active" : ""}>
              <Link href="/install" className="lnk">
                Install
              </Link>
            </li>
          </ul>
        </div>
      </div>
    </header>
  );
};
export default Header;
