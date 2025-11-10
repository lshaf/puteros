import PageBanner from "@/components/PageBanner";
import GlitcheLayout from "@/layouts/GlitcheLayout";
import "@css/template-dark/dark.css";

const Feature = () => {
  return (
    <GlitcheLayout dark={true}>
      <PageBanner pageLink={"feature"} pageName={"Feature"} dark={true} />
    </GlitcheLayout>
  );
};
export default Feature;
