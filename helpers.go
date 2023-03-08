package main

import (
	"errors"
	"net/http"
	"strconv"
	"strings"
)

func min(a string, b string) string {
	if a < b {
		return a
	} else {
		return b
	}
}

func serialize[T int | uint | uint32](arr []T) string {
	str := ""

	for i, elem := range arr {
		if i > 0 {
			str += ","
		}

		str += strconv.FormatUint(uint64(elem), 10)
	}

	return str
}

func deserialize(strs string) []uint32 {
	arr := strings.Split(strs, ",")
	returnVal := make([]uint32, len(arr))

	for i, elem := range arr {
		converted, err := strconv.ParseUint(elem, 10, 32)
		if err != nil {
			converted = 0
			continue
		}

		returnVal[i] = uint32(converted)
	}

	return returnVal
}

func getValues(r *http.Request, args ...string) (map[string]string, error) {
	returnValues := make(map[string]string)

	for _, elem := range args {
		values, ok := r.URL.Query()[elem]
		if !ok {
			return returnValues, errors.New(elem + " not found")
		}

		returnValues[elem] = values[0]
	}

	return returnValues, nil
}
