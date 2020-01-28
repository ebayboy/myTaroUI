import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtForm, AtSlider } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtSliderTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { value: 50 }
    }

    handleChange(e) {
        this.setState({ value: e.value })
        console.log("change e.value:", e.value, "state.value:", this.state.value)
    }

    handleChangeing(e) {
        this.setState({ value: e.value })
        console.log("changing e.value:", e.value, "state.value:", this.state.value)
    }

    render() {
        return (
            <View>
                <AtSlider
                    activeColor='#4285F4' backgroundColor='#BDBDBD' blockColor='#4285F4' blockSize={24}
                    showValue={true}
                    min={0}
                    max={100}
                    step={1}
                    value={this.state.value}
                    onChanging={this.handleChangeing.bind(this)}
                    onChange={this.handleChange.bind(this)}
                ></AtSlider>
            </View>
        )
    }
}