"use client";
import Link from "next/link";
import { useEffect } from "react";

const PageBanner = ({ pageName, pageLink, dark }) => {
  useEffect(() => {
    var sections = document.querySelectorAll(".section");
    sections.forEach(function (section) {
      section.addEventListener("click", function (event) {
        if (event.target.classList.contains("mouse")) {
          var height = window.innerHeight;
          var scrollOptions = {
            top: height - 150,
            behavior: "smooth", // For smooth scrolling (modern browsers)
          };

          if ("scrollBehavior" in document.documentElement.style) {
            // If smooth scrolling is supported
            window.scrollTo(scrollOptions);
          } else {
            // Fallback for browsers that don't support smooth scrolling
            window.scrollTo(scrollOptions.top, scrollOptions.top);
          }
        }
      });
    });
  }, []);

  return (
    <div className="section started" style={{ height: "96vh" }}>
      <div className="centrize full-width">
        <div className="vertical-center">
          <div className="started-content">
            <div className="h-title glitch-effect" data-text={pageName}>
              {pageName}
            </div>
            <div className="h-subtitle typing-bread">
              <p>
                <Link href="/">Home</Link> /{" "}
                <Link href={pageLink}>{pageName}</Link>
              </p>
            </div>
            <span className="typed-bread" />
          </div>
        </div>
      </div>
      <a href="#" className="mouse_btn">
        <span className="ion ion-mouse mouse" />
      </a>
    </div>
  );
};
export default PageBanner;
