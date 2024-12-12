// Copyright 2024 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import SwiftUI

import FirebaseAuth

struct LoginView: View {
  @Environment(\.dismiss) private var dismiss

  @State private var email: String = ""
  @State private var password: String = ""

  private weak var delegate: (any LoginDelegate)?

  init(delegate: (any LoginDelegate)? = nil) {
    self.delegate = delegate
  }

  private func login() {
    Task {
      do {
        _ = try await AppManager.shared
          .auth()
          .signIn(withEmail: email, password: password)
        await MainActor.run {
          dismiss()
          delegate?.loginDidOccur(resolver: nil)
        }
        // TODO(ncooke3): Investigate possible improvements.
//      } catch let error as AuthErrorCode
//        where error.code == .secondFactorRequired {
//        // error as? AuthErrorCode == nil because AuthErrorUtils returns generic
//        /Errors
//        // https://firebase.google.com/docs/auth/ios/totp-mfa#sign_in_users_with_a_second_factor
      } catch let error as NSError
        where error.code == AuthErrorCode.secondFactorRequired.rawValue {
        let mfaKey = AuthErrorUserInfoMultiFactorResolverKey
        guard let resolver = error.userInfo[mfaKey] as? MultiFactorResolver else { throw error }
        await MainActor.run {
          dismiss()
          delegate?.loginDidOccur(resolver: resolver)
        }
      } catch {
        print(error.localizedDescription)
      }
    }
  }

  private func createUser() {
    Task {
      do {
        _ = try await AppManager.shared.auth().createUser(
          withEmail: email,
          password: password
        )
        // Sign-in was successful.
        await MainActor.run {
          dismiss()
          delegate?.loginDidOccur(resolver: nil)
        }
      } catch {
        // TODO(ncooke3): Implement error display.
        print(error.localizedDescription)
      }
    }
  }
}

extension LoginView {
  var body: some View {
    VStack(alignment: .leading) {
      Text(
        "Login or create an account using the Email/Password auth " +
          "provider.\n\nEnsure that the Email/Password provider is " +
          "enabled on the Firebase console for the given project."
      )
      .fixedSize(horizontal: false, vertical: true)
      .padding(.bottom)

      TextField("Email", text: $email)
        .textFieldStyle(SymbolTextField(symbolName: "person.crop.circle"))

      TextField("Password", text: $password)
        .textFieldStyle(SymbolTextField(symbolName: "lock.fill"))
        .padding(.bottom)

      Group {
        Button(action: login) {
          Text("Login")
            .bold()
        }
        .buttonStyle(CustomButtonStyle(backgroundColor: .orange, foregroundColor: .white))

        Button(action: createUser) {
          Text("Create Account")
            .bold()
        }
        .buttonStyle(CustomButtonStyle(backgroundColor: .primary, foregroundColor: .orange))
      }
      .disabled(email.isEmpty || password.isEmpty)

      Spacer()
    }
    .padding()
  }
}

private struct SymbolTextField: TextFieldStyle {
  let symbolName: String

  func _body(configuration: TextField<Self._Label>) -> some View {
    HStack {
      Image(systemName: symbolName)
        .foregroundColor(.orange)
        .imageScale(.large)
        .padding(.leading)
      configuration
        .padding([.vertical, .trailing])
    }
    .background(Color(uiColor: .secondarySystemBackground))
    .cornerRadius(14)
    .textInputAutocapitalization(.never)
  }
}

private struct CustomButtonStyle: ButtonStyle {
  let backgroundColor: Color
  let foregroundColor: Color
  func makeBody(configuration: Configuration) -> some View {
    HStack {
      Spacer()
      configuration.label
      Spacer()
    }
    .padding()
    .background(backgroundColor, in: RoundedRectangle(cornerRadius: 14))
    .foregroundStyle(foregroundColor)
    .opacity(configuration.isPressed ? 0.5 : 1) // is disabled?
  }
}

#Preview {
  LoginView()
}
