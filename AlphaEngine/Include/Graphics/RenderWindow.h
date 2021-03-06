#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

/**
Copyright 2014-2015 Jason R. Wendlandt

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include<stdio.h>
#include<stdlib.h>

#include <SDL.h>
#include <SDL_video.h>
#include <SDL_syswm.h>

namespace alpha
{
	class RenderWindow
	{
	public:
		RenderWindow();
		virtual ~RenderWindow();

        bool Initialize(int windowWidth, int windowHeight);
        bool Update(double currentTime, double elapsedTime);
        bool Shutdown();

        void Render();

        SDL_Window * GetWindow() const;
        SDL_SysWMinfo GetWindowInfo() const;

    private:
        SDL_Window * m_pWindow;
	};
}

#endif // RENDER_WINDOW_H
