/*
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin Gorka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef PLAINCRAFT_CORE_GAME_OBJECT
#define PLAINCRAFT_CORE_GAME_OBJECT

#include "../common.hpp"
#include "../physics/physics_object.hpp"
#include <plaincraft_render_engine.hpp>

namespace plaincraft_core {
	using namespace plaincraft_render_engine;
	
	class GameObject
	{
	private:
		std::shared_ptr<Drawable> drawable_;
		std::shared_ptr<PhysicsObject> physics_object_;
		std::string name_;
		
		bool is_active_;

		static uint32_t next_id_;
		uint32_t unique_id_;

	public:
		GameObject();
		virtual ~GameObject();

		GameObject(const GameObject& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;

		GameObject(GameObject&& other) noexcept;
		GameObject& operator=(GameObject&& other) noexcept;

		void SetDrawable(std::shared_ptr<Drawable> drawable);
		std::shared_ptr<Drawable> GetDrawable() const;

		void SetPhysicsObject(std::shared_ptr<PhysicsObject> physics_object);
		std::shared_ptr<PhysicsObject> GetPhysicsObject() const;

		void SetColor(Vector3d color);
		Vector3d GetColor();

		void SetName(std::string name);
		std::string GetName() const;
		
		void SetIsActive(bool is_active);
		bool GetIsActive() const;

		uint32_t GetUniqueId() const;
	};
}

#endif // PLAINCRAFT_CORE_GAME_OBJECT