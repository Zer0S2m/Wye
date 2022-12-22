from setuptools import (
    setup, Extension
)


def main():
    setup(
        name="wye_temp_engine",
        version="1.0.0",
        description="Python template engine interface for Wye",
        ext_modules=[
            Extension(
                "wye_temp_engine",
                ["./wye/lib/temp_engine/src/core/core.c"],
                include_dirs=['./wye/lib/temp_engine/src/']
            )
        ]
    )


if __name__ == "__main__":
    main()
