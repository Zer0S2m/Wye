from distutils.core import (
    setup, Extension
)


def main():
    setup(
		name = "wye_serializers",
        version = "1.0.0",
        description = "Python interface for the Wye",
        author = "Test Test",
        author_email = "test@gmail.com",
        ext_modules = [
			Extension(
                "wye_serializers",
                [".\\wye\\serializers\\src\\core\\core.c"],
                include_dirs = ['.\\wye\\serializers\\src\\']
            )
		],
		package_dir = {"": "wye"}
	)


if __name__ == "__main__":
    main()
