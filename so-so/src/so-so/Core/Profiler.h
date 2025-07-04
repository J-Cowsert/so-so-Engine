#pragma once
#include "Core.h"

#define SS_ENABLE_PROFILING 0

#if SS_ENABLE_PROFILING
    #include <Tracy.hpp>
    #define SS_PROFILE_FRAME_MARK() FrameMark
    #define SS_PROFILE_FUNCTION() ZoneScoped
    #define SS_PROFILE_SCOPE(name) ZoneScopedN(name)
#else
    #define SS_PROFILE_FRAME_MARK()
    #define SS_PROFILE_SCOPE(name)
    #define SS_PROFILE_FUNCTION()
#endif

#if 0
#include "Timer.h"
#include <vector>
#include <string>
#include <mutex>

#include "imgui.h"

namespace soso {

	enum class ProfileType { Function, Scope };

	struct ProfileData {

		std::string ID;
		float Duration;
	};


	class Profiler {
	public:

		static void AddProfile(ProfileData&& data) {
			std::lock_guard<std::mutex> lock(s_Mutex);
			s_Profiles.emplace_back(std::move(data));
		}

		static void OnImGuiRender() {

            std::vector<ProfileData> copy;
            {
                std::lock_guard<std::mutex> lock(s_Mutex);
                if (s_Profiles.empty()) return;
                copy = std::move(s_Profiles);
                s_Profiles.clear();
            }

            struct Agg { float total = 0, max = 0; int count = 0; };
            std::unordered_map<std::string, Agg> aggMap;

            for (auto& pd : copy) {

                auto& a = aggMap[pd.ID];
                a.total += pd.Duration;
                a.max = std::max(a.max, pd.Duration);
                a.count++;
            }

            std::vector<std::pair<std::string, Agg>> list;
            list.reserve(aggMap.size());

            for (auto& kv : aggMap)
                list.emplace_back(kv.first, kv.second);

            std::sort(list.begin(), list.end(), [](auto& a, auto& b) {
                return (a.second.total / a.second.count) > (b.second.total / b.second.count);
                });

            float maxAvg = 0;
            for (auto& it : list) {
                float avg = it.second.total / it.second.count;
                maxAvg = std::max(maxAvg, avg);
            }

            if (ImGui::Begin("Profiler")) {

                ImGui::BeginTable("##profilerTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
                ImGui::TableSetupColumn("Scope");
                ImGui::TableSetupColumn("Avg (ms)", ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("Max (ms)", ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("Timeline");
                ImGui::TableHeadersRow();

                for (auto& item : list) {

                    const auto& name = item.first;
                    float avg = item.second.total / item.second.count;
                    float maxv = item.second.max;
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(name.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.3f", avg);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%.3f", maxv);
                    ImGui::TableSetColumnIndex(3);
                    float norm = maxAvg > 0 ? avg / maxAvg : 0;
                    ImGui::ProgressBar(norm, ImVec2(-1, 0), "");
                }

                ImGui::EndTable();
                ImGui::End();
            }
        }

	private:
		inline static std::vector<ProfileData> s_Profiles;
		inline static std::mutex s_Mutex;
	};

	class ScopedProfile {
	public:
		ScopedProfile(const char* name)
			: m_Name(name), m_Timer() {
		}

		~ScopedProfile() {
			float ms = m_Timer.ElapsedMillis();
			Profiler::AddProfile({ m_Name, ms });
		}

	private:
		const char* m_Name;
		Timer m_Timer;
	};
}

#endif