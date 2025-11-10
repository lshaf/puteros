import PageBanner from "@/components/PageBanner";
import GlitcheLayout from "@/layouts/GlitcheLayout";
import "@css/template-dark/dark.css";

const Resume = () => {
  return (
    <GlitcheLayout dark={true}>
      <PageBanner pageLink={"install"} pageName={"Install"} dark={true} />
    </GlitcheLayout>
  );
};
export default Resume;
