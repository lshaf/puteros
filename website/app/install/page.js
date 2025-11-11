import PageBanner from "@/components/PageBanner";
import GlitcheLayout from "@/layouts/GlitcheLayout";
import "@css/template-dark/dark.css";
import FlashFirmware from "@/components/FlashFirmware";

const Resume = () => {
  return (
    <GlitcheLayout dark={true}>
      <PageBanner pageLink={"install"} pageName={"Install"} dark={true} />
      <FlashFirmware />
    </GlitcheLayout>
  );
};
export default Resume;
