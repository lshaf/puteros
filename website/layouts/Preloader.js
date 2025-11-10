"use client";
import { useEffect, useState } from "react";

const Preloader = () => {
  useEffect(() => {
    var preInner = document.querySelector(".preloader .pre-inner");

    // Fade out .pre-inner element
    fadeOut(preInner, 800, function () {
      // Preload hide
      var preloader = document.querySelector(".preloader");
      preloader.style.display = "none";

      // Add 'loaded' class to body
      document.body.classList.add("loaded");
    });

    function fadeOut(element, duration, callback) {
      var opacity = 1;
      var interval = 1000 / 60; // 60 frames per second
      var step = opacity / (duration / interval);

      function updateOpacity() {
        opacity -= step;
        element.style.opacity = opacity;

        if (opacity <= 0) {
          clearInterval(fadeInterval);
          if (typeof callback === "function") {
            callback();
          }
        }
      }

      var fadeInterval = setInterval(updateOpacity, interval);
    }
  }, []);

  const [text, setText] = useState("loading ...");
  useEffect(() => {
    window.location.pathname.includes("rtl")
      ? setText("جار التحميل...")
      : "loading ...";
  }, []);

  return (
    <div className="preloader">
      <div className="centrize full-width">
        <div className="vertical-center">
          <div className="pre-inner">
            <div className="load typing-load">
              <p>{text}</p>
            </div>
            <span className="typed-load" />
          </div>
        </div>
      </div>
    </div>
  );
};
export default Preloader;
