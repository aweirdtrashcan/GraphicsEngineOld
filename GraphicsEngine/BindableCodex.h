#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include "IBindable.h"

class BindableCodex {
public:
	static BindableCodex& Get() {
		static BindableCodex m_Instance;
		return m_Instance;
	}

	template<typename T, typename... Args>
	static std::shared_ptr<T> Resolve(Args&&... args) {
		return ResolveImpl<T>(std::forward<Args>(args)...);
	}
private:
	template<typename T, typename... Args>
	static std::shared_ptr<T> ResolveImpl(Args&&... args) {
		auto key = T::GenerateKey(std::forward<Args>(args)...);
		if (Get().bindables.find(key) == Get().bindables.end()) {
			std::shared_ptr<T> b = std::make_shared<T>(std::forward<Args>(args)...);
			Get().bindables[key] = std::move(b);
		}

		return std::static_pointer_cast<T, IBindable>(Get().bindables[key]);
	}
private:
	std::unordered_map<std::string, std::shared_ptr<IBindable>> bindables;
};