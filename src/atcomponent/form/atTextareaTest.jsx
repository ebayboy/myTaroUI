import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtForm, AtTextarea } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtTextareaTest extends Component {
    constructor() {
        super(...arguments)

        this.state = { value: '' }
    }

    handleChange(event) {
        console.log("change value:", event.target.value)
        this.setState({ value: event.target.value })
    }

    render() {
        return (
            <View>
                <AtTextarea
                    maxLength={200}
                    value={this.state.value}
                    placeholder={'你的问题是...'}
                    onChange={this.handleChange.bind(this)}
                ></AtTextarea>
            </View>
        )
    }
}