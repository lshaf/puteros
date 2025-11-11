import PageBanner from "@/components/PageBanner";
import GlitcheLayout from "@/layouts/GlitcheLayout";
import "@css/template-dark/dark.css";
import OverallFeature from "@/components/OverallFeature";
import DetailFeature from "@/components/DetailFeature";

const Feature = () => {
  return (
    <GlitcheLayout dark={true}>
      <PageBanner pageLink={"feature"} pageName={"Feature"} dark={true} />
      <OverallFeature />
      <DetailFeature />
    </GlitcheLayout>
  );
};
export default Feature;
