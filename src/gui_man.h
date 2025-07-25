#include <map>
#include <functional>
#include <raygui.h>

struct Operation {
	bool _is_locked = false;
	std::function<int()> invokation;
};



class _GUI {
public:
	_GUI() {
	Operation a{
	true,
	[]() -> int {
		DrawText("Hello!",  1, 1, 14, RED);
		return 0;
	}
		};
	}
	void upd() {
		for (auto& [key, op] : _locked_states) {
			this->_upd_single(key);
		}
	}
private:
	std::map<int, Operation> _locked_states;

	void _upd_single(size_t id) {
		if (!this->_locked_states[id]._is_locked)
			GuiEnable();
		this->_locked_states[id].invokation();
		GuiDisable();
	}
};