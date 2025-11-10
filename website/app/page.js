import Hero from "@/components/Hero";
import GlitcheLayout from "@/layouts/GlitcheLayout";
import "@css/template-dark/dark.css";
const page = () => {
  return (
    <GlitcheLayout dark={true}>
      <Hero />
    </GlitcheLayout>
  );
};
export default page;
