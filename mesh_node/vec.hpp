template<typename T>
struct vec
{
	T values[3];

	T &operator[](size_t i) { return values[i]; }
	const T &operator[](size_t i) const { return values[i]; }

	inline T &x() { return values[0]; }
	inline const T &x() const { return values[0]; }

	inline T &y() { return values[1]; }
	inline const T &y() const { return values[1]; }

	inline T &z() { return values[2]; }
	inline const T &z() const { return values[2]; }

	T *begin() { return static_cast<T *>(values); }
	const T *begin() const { return static_cast<const T *>(values); }

	T *end() { return static_cast<T *>(values) + 3; }
	const T *end() const { return static_cast<const T *>(values) + 3; }
};
