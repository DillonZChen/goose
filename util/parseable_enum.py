from enum import Enum


class ParseableEnum(Enum):
    @classmethod
    def choices(cls):
        return [item for item in cls]

    @classmethod
    def parse(cls, value):
        if isinstance(value, cls):
            return value
        return cls(value)

    def __str__(self):
        return self.value
