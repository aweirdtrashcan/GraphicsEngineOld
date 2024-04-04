#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include "IBindable.h"

class BindableCodex {
public:
	BindableCodex() = delete;

	template<typename T, typename... Args>
	static std::shared_ptr<T> Resolve(Args&&... args) {
		return ResolveImpl(std::forward(args));
	}
private:
	template<typename T, typename... Args>
	static std::shared_ptr<T> ResolveImpl(Args&&... args) {
		std::string key = T::Resolve(args);
		if (bindables.find(key) == bindables.end()) {
			std::shared_ptr<T> b = std::make_shared<T>(std::forward(args));
			bindables[key] = std::move(b);
		}

		return bindables[key];
	}
private:
	std::unordered_map<std::string, std::shared_ptr<IBindable>> bindables;
};