from setuptools import (
    setup, Extension
)


def main():
    setup(
        name="wye_serializers",
        version="1.0.0",
        description="Python serializer interface for Wye",
        ext_modules=[
            Extension(
                "wye_serializers",
                ["./wye/serializers/src/core/core.c"],
                include_dirs=['./wye/serializers/src/']
            )
        ]
    )


if __name__ == "__main__":
    main()
