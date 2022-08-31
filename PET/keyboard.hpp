#pragma once
#include "type_aliases.hpp"

struct GLFWwindow;

class Keyboard
{
public:
	Keyboard() = default;

	u8 getKey() const { return m_rows[m_row] ^ 0xFF; }
	u8 getRow() const { return m_row; }
	void setRow(u8 row) { m_row = row; }

	static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
private:
	u8 m_row = 0;
	u8 m_rows[10]{};
};
