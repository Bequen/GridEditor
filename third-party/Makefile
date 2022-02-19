# This makefile simply builds each project
# That's all

.PHONY: imgui
imgui:
	@if [ -d "./imgui/" ]; then \
		read -p "Redownload? [y/N]> " isDownloading; \
		[[ $$isDownloading == [Yy]* ]] && (rm -rf ./imgui/ && git clone https://github.com/ocornut/imgui) || echo "Skipping"; \
	else \
		git clone https://github.com/ocornut/imgui; \
	fi

	cd ./imgui/ && clang ./*.cpp -c
	cd ./imgui/ && clang -DIMGUI_IMPL_OPENGL_LOADER_GLAD=1 -I../glad/ -I./../glfw/include/ -L../glfw/src/ -I./ ./backends/imgui_impl_glfw.cpp ./backends/imgui_impl_opengl3.cpp -c
	ar rvs -c ./imgui/libimgui.a `ls ./imgui/*.o`

imguizmo:
	@if [ -d "./ImGuizmo/" ]; then \
		read -p "Redownload? [y/N]> " isDownloading; \
		[[ $$isDownloading == [Yy]* ]] && (rm -rf ./ImGuizmo/ && git clone https://github.com/CedricGuillemet/ImGuizmo) || echo "Skipping"; \
	else \
		git clone https://github.com/CedricGuillemet/ImGuizmo; \
	fi

	cd ./ImGuizmo/ && clang -I ../imgui/ -c ./*.cpp
	ar rvs -c ./ImGuizmo/libimguizmo.a `ls ./ImGuizmo/*.o`
	@echo "Done";

glm:
	git clone https://github.com/g-truc/glm

glfw:
	git clone https://github.com/glfw/glfw