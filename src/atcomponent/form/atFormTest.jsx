import Taro, { Component, showActionSheet } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtForm, AtButton, AtInput } from 'taro-ui'
import "taro-ui/dist/style/index.scss"

export default class AtFormTest extends Component {
    constructor() {
        super(...arguments)

        this.state = { value: 'init value' }
    }

    handleClickSubmit(event) {
        console.log("click", event)
    }

    handleClickReset(event) {
        this.setState({ value: '' })
        console.log("reset state to :", this.state, event)
    }

    handleChange(value) {
        this.setState({ value: value })
        console.log("change", this.state)
    }

    render() {
        return (
            <View>
                <AtForm
                    onSubmit={this.handleClickSubmit.bind(this)}
                    onReset={this.handleClickReset.bind(this)}>
                    <AtInput
                        type='text'
                        name='value'
                        title='文本'
                        value={this.state.value}
                        onChange={this.handleChange.bind(this)}
                    ></AtInput>
                    <AtButton formType='submit'>submit</AtButton>
                    <AtButton formType='reset'>reset</AtButton>
                </AtForm>
            </View>
        )
    }
}