import Preloader from "@/layouts/Preloader";
import "@css/animate.css";
import "@css/glitche-basic.css";
import "@css/glitche-layout.css";
import "@css/ionicons.css";
import "@css/magnific-popup.css";
import "@css/template-colors/blue.css";
import { Roboto_Mono } from "next/font/google";
import "./globals.css";
import State from "../context/context";

const robotoMono = Roboto_Mono({
  subsets: ["latin"],
  weight: ["100", "300", "400", "500", "700"],
  variable: "--font-roboto",
  display: "swap",
});
export const metadata = {
  title: "PuterOS - Pentesting Firmware",
  description: "ESP32 S3 based Pentesting Firmware",
};

export default function RootLayout({ children }) {
  return (
    <html lang="en">
      <body className={robotoMono.variable}>
        {/* Preloader */}
        <Preloader />
        <State>{children} </State>
      </body>
    </html>
  );
}
