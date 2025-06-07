#include "ShaderEditor.h"

#include "so-so/Renderer/Renderer.h"

#include "misc/cpp/imgui_stdlib.h"

#include <fstream>
#include <ostream>
#include <istream>

namespace Utils {

    static std::string ReadFromDisk(const std::string& path) {

        std::string result;
        std::ifstream in(path, std::ios::in);
        
        if (in) {

            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            if (size != std::streampos(-1)) {
                result.resize((size_t)size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], (size_t)size);

                auto actuallyRead = in.gcount();
                result.resize((size_t)actuallyRead);
            }
            else {
                SS_CORE_ERROR("Could not read from file '{0}'", path);
            }
        }
        else {
            SS_CORE_ERROR("Could not open file '{0}'", path);
        }

        return result;
    }

    static void WriteToDisk(const std::string& path, const std::string& data) {

        std::ofstream out(path, std::ios::out | std::ios::trunc | std::ios::binary);

        if (out) {

            out.write(data.data(), data.size());
        }
        else {
            SS_CORE_ERROR("Could not open file '{0}'", path);
        }
    }
}

ShaderEditor::ShaderEditor() {

	m_Shaders = soso::Renderer::GetShaderLibrary()->GetAll();

}

void ShaderEditor::Draw() {

    if (!ImGui::Begin("Shader Editor"))
    {
        ImGui::End();
        return;
    }

    // ── Left pane: shader list ────────────────────────────────────────────

    ImGui::BeginChild("Shaders", ImVec2(200, 0), true);

    for (auto& s : m_Shaders)
        
        if (ImGui::Selectable(s->GetName().c_str(), s == m_Selected.Shader)) {


            m_Selected.SourceBuffer.clear();

            m_Selected.Shader = s;
            m_Selected.SourceBuffer = Utils::ReadFromDisk(s->GetFilepath());
           
        }


    if (m_Selected.Shader) {

        ImGui::NewLine();
        ImGui::Separator();

        if (ImGui::Button("Save & Compile")) {

            Utils::WriteToDisk(m_Selected.Shader->GetFilepath(), m_Selected.SourceBuffer);

            m_Selected.Shader->Reload();
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // ── Right pane: source viewer ─────────────────────────────────────────
    ImGui::BeginChild("Source", ImVec2(0, 0), false);
    if (m_Selected.Shader) {

        auto& src = m_Selected.SourceBuffer;

        ImGui::InputTextMultiline("##code", &src, ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_AllowTabInput);
        
        
        if (ImGui::IsItemEdited()) {
            
        }

        ImGui::SameLine();
        if (ImGui::Button("Reload from Disk")) { /*…*/ }
        
        
    }
    ImGui::EndChild();

    ImGui::End();
}
